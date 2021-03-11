#include "OS_RBtree.h"
#include <assert.h>
#include <stdlib.h>

rbnode __T_nil = { .p = 0, .l = 0, .r = 0, .col = BLACK, .sub_sz = 0 };
iterator T_nil = &__T_nil;

#ifdef DEFAULT_KEY_TYPE
  int comp_keys(const key_type A, const key_type B) {
      return A == B? 0 : A < B? -1 : +1;
  }
#endif

iterator newnode(key_type key, value_type val) {
    iterator nd = malloc(sizeof *nd);
    *(key_type *)&nd->key = key; nd->val = val;
    nd->p = nd->l = nd->r = T_nil;
    nd->col = RED; nd->sub_sz = 1;
    return nd;
}

Rb_tree newrbtree() {
    return (Rb_tree) {
        .root = T_nil, .size = 0,
        .begin = T_nil, .rbegin = T_nil
    };
}

iterator next(iterator it) {
#ifdef assert
    assert(it != T_nil && "attempt to increment T_nil");
#endif
    if(it->r != T_nil) {
        it = it->r;
        while(it->l != T_nil)
            it = it->l;
    } else {
        while(it->p != T_nil && it == it->p->r)
            it = it->p;
        it = it->p;
    } return it;
}

iterator prev(iterator it) {
#ifdef assert
    assert(it != T_nil && "attempt to decrement T_nil");
#endif
    if(it->l != T_nil) {
        it = it->l;
        while(it->r != T_nil)
            it = it->r;
    } else {
        while(it->p != T_nil && it == it->p->l)
            it = it->p;
        it = it->p;
    } return it;
}

iterator find(Rb_tree *T, key_type key) {
    iterator x = T->root;
    while(x != T_nil) {
        int o = comp_keys(key, x->key);
        if(o == 0) return x;
        x = o < 0? x->l : x->r;
    } return T_nil;
}

int contains(Rb_tree *T, key_type key) {
    return find(T, key) != T_nil;
}

iterator lower_bound(Rb_tree *T, key_type key) {
    iterator lb = T_nil, cur = T->root;
    while(cur != T_nil) {
        if(comp_keys(cur->key, key) >= 0)
            lb = cur, cur = cur->l;
        else cur = cur->r;
    } return lb;
}

iterator upper_bound(Rb_tree *T, key_type key) {
    iterator ub = T_nil, cur = T->root;
    while(cur != T_nil) {
        if(comp_keys(cur->key, key) > 0)
            ub = cur, cur = cur->l;
        else cur = cur->r;
    } return ub;
}

int order_of_key_it(Rb_tree *T, iterator it) {
#ifdef assert
    assert(it != T_nil && "ook: invalid iterator or key");
#endif
    int res = it->l->sub_sz;
    while(it != T->root) {
        if(it == it->p->r)
            res += it->p->l->sub_sz + 1;
        it = it->p;
    } return res;
}
int order_of_key_k(Rb_tree *T, key_type key) {
    iterator it = lower_bound(T, key);
    return it == T_nil? T->size : order_of_key_it(T, it);
}
iterator find_by_order(Rb_tree *T, int k) {
#ifdef assert
    assert(0 <= k && k < T->size && "fbo: invalid k");
#endif
    iterator it = T->root;
    while(1) {
        if(it->l->sub_sz == k)
            return it;
        if(k > it->l->sub_sz)
            k -= it->l->sub_sz + 1, it = it->r;
        else it = it->l;
    }
}

void transplant(iterator *root, iterator x, iterator y) {
    if(x->p == T_nil) *root = y;
    else if(x == x->p->l) x->p->l = y;
    else x->p->r = y;
    y->p = x->p;
}

void left_rotate(iterator *root, iterator x) {
    iterator y = x->r;
    x->r = y->l;
    if(y->l != T_nil)
        y->l->p = x;
    y->p = x->p;
    if(y->p == T_nil)
        *root = y;
    else if(x == x->p->l)
        x->p->l = y;
    else
        x->p->r = y;
    y->l = x;
    x->p = y;
    y->sub_sz = x->sub_sz;
    x->sub_sz = x->l->sub_sz + x->r->sub_sz + 1;
}

void right_rotate(iterator *root, iterator y) {
    iterator x = y->l;
    y->l = x->r;
    if(x->r != T_nil)
        x->r->p = y;
    x->p = y->p;
    if(y->p == T_nil)
        *root = x;
    else if(y == y->p->l)
        y->p->l = x;
    else
        y->p->r = x;
    x->r = y;
    y->p = x;
	  x->sub_sz = y->sub_sz;
    y->sub_sz = y->l->sub_sz + y->r->sub_sz + 1;
}

void fix_insert(iterator *root, iterator z) {
    while(z->p->col == RED) {
        if(z->p == z->p->p->l) {
            iterator y = z->p->p->r;
            if(y->col == RED) {
                z->p->col = y->col = BLACK;
                z->p->p->col = RED;
                z = z->p->p;
            } else {
                if(z == z->p->r)
                    left_rotate(root, z = z->p);
                z->p->col = BLACK;
                z->p->p->col = RED;
                right_rotate(root, z->p->p);
            }
        } else {
            iterator y = z->p->p->l;
            if(y->col == RED) {
                z->p->col = y->col = BLACK;
                z->p->p->col = RED;
                z = z->p->p;
            } else {
                if(z == z->p->l)
                    right_rotate(root, z = z->p);
                z->p->col = BLACK;
                z->p->p->col = RED;
                left_rotate(root, z->p->p);
            }
        }
    } (*root)->col = BLACK;
}

insert_erase_t _insert(Rb_tree *T, kvargs kv) {
	  iterator y = T_nil, x = T->root;
    while(x != T_nil) {
        y = x;
        x->sub_sz++;
        int o = comp_keys(kv.key, x->key);
        if(o == 0) {
            while(y != T_nil) {
                y->sub_sz--;
                y = y->p;
            } return (insert_erase_t){0, x};
        } x = o < 0? x->l : x->r;
    }
    iterator z = newnode(kv.key, kv.val);
    z->p = y; T->size++;
    if(y == T_nil) T->root = z;
    else if(comp_keys(z->key, y->key) < 0) y->l = z;
    else y->r = z;
    fix_insert(&T->root, z);
    if(T->begin == T_nil
        || comp_keys(z->key, T->begin->key) < 0)
            T->begin = z;
    if(T->rbegin == T_nil
        || comp_keys(z->key, T->begin->key) > 0)
            T->rbegin = z;
    return (insert_erase_t){1, z};
}

void fix_erase(Rb_tree *T, iterator x) {
    while(x != T->root && x->col == BLACK) {
        if(x == x->p->l) {
            iterator w = x->p->r;
            if(w->col == RED) {
                w->col = BLACK;
                x->p->col = RED;
                left_rotate(&T->root, x->p);
                w = x->p->r;
            }
            if(w->l->col == BLACK && w->r->col == BLACK) {
                w->col = RED;
                x = x->p;
            } else {
                if(w->r->col == BLACK) {
                    w->l->col = BLACK;
                    w->col = RED;
                    right_rotate(&T->root, w);
                    w = x->p->r;
                }
                w->col = x->p->col;
                x->p->col = BLACK;
                w->r->col = BLACK;
                left_rotate(&T->root, x->p);
                x = T->root;
            }
        } else {
            iterator w = x->p->l;
            if(w->col == RED) {
                w->col = BLACK;
                x->p->col = RED;
                right_rotate(&T->root, x->p);
                w = x->p->l;
            }
            if(w->r->col == BLACK && w->l->col == BLACK) {
                w->col = RED;
                x = x->p;
            } else {
                if(w->l->col == BLACK) {
                    w->r->col = BLACK;
                    w->col = RED;
                    left_rotate(&T->root, w);
                    w = x->p->l;
                }
                w->col = x->p->col;
                x->p->col = BLACK;
                w->l->col = BLACK;
                right_rotate(&T->root, x->p);
                x = T->root;
            }
        }
    } x->col = BLACK;
}

iterator _erase_it(Rb_tree *T, iterator z) {
#ifdef assert
    assert(z != T_nil);
#endif
    T->size--;
    iterator nxt = next(z);
    if(z == T->begin) T->begin = nxt;
    if(z == T->rbegin) T->rbegin = prev(z);
    for(iterator x = z; x != T_nil; x = x->p)
        x->sub_sz--;

    iterator x, y = z;
    char orig_col = z->col;
    if(z->l == T_nil) {
        x = z->r; transplant(&T->root, z, z->r);
    } else if(z->r == T_nil) {
        x = z->l; transplant(&T->root, z, z->l);
    } else {
        y = z->r;
        while(y->l != T_nil)
            y = y->l;
        orig_col = y->col;
        x = y->r;
        if(y->p == z) {
            x->p = y;
        } else {
            transplant(&T->root, y, y->r);
            y->r = z->r;
            y->r->p = y;
        }
        transplant(&T->root, z, y);
        y->l = z->l;
        y->l->p = y;
        y->col = z->col;
    }

    if(orig_col == BLACK)
        fix_erase(T, x);

    return nxt;
}

insert_erase_t _erase_k(Rb_tree *T, key_type key) {
    iterator it = find(T, key);
    if(it == T_nil) return (insert_erase_t){0, T_nil};
    return (insert_erase_t){1, _erase_it(T, it)};
}