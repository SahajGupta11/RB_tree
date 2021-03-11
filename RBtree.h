/*
    C implementation of red-black tree, taking inspiration from
    Introduction to Algorithms (Third edition) and C++ STL.
*/
#ifndef RB_TREE_H 
#define RB_TREE_H 

#define BLACK 1
#define RED 0

// Following may be defined with custom types as required prior to
// including the header file. If so, comp_keys must also be defined.
#ifndef KEY_TYPE
  #define KEY_TYPE int
  #define DEFAULT_KEY_TYPE 
#endif
#ifndef VALUE_TYPE
  #define VALUE_TYPE int
#endif

typedef KEY_TYPE key_type;
typedef VALUE_TYPE value_type;

// compare function:= -1: A < B, 0: A == B, 1: A > B
int comp_keys(const key_type A, const key_type B);

typedef struct {
    key_type key;
    value_type val;
} kvargs;

typedef struct rbnode {
    struct rbnode *p, *l, *r;
    char col;
    const key_type key;
    value_type val;
} rbnode;

typedef rbnode* iterator;

typedef struct {
    int success;    // 1: inserted/erased, 0: otherwise
    iterator it;    // iterator to insert: inserted node, erase: next node
} insert_erase_t;

typedef struct {    // constant time access to root, begin and rbegin
    iterator root, begin, rbegin;   // begin: leftmost node
    int size;                       // rbegin: rightmost node
} Rb_tree;                          // size: number of nodes

extern rbnode __T_nil;
extern iterator T_nil;                                  // behaves like NULL, but typed as an iterator

iterator newnode(key_type key, value_type val);         // factory method RB node
Rb_tree newrbtree();                                    // factory method RB tree

iterator next(iterator it);                             // inorder traversal, ++it in O(1) amortized, returns T_nil if not found
iterator prev(iterator it);                             // inorder traversal, --it in O(1) amortized, returns T_nil if not found

iterator find(Rb_tree *T, key_type key);                // find a key in O(logn), returns T_nil if not found
int contains(Rb_tree *T, key_type key);                 // check existence of a key in O(logn)
iterator lower_bound(Rb_tree *T, key_type key);         // find lower bound key of a value in O(logn), returns T_nil if not found
iterator upper_bound(Rb_tree *T, key_type key);         // find upper bound key of a value in O(logn), returns T_nil if not found

#define insert(T, ...) _insert(T, (kvargs){__VA_ARGS__})    // insert(&T, key, val) or insert(&T, key)
insert_erase_t _insert(Rb_tree *T, kvargs kv);          // insertions in O(logn), returns {success: 0/1, iterator to newly inserted node}
#define erase(T, x)                                     /* erase(&T, iterator) or erase(&T, key) */ \
    _Generic((x),               \
        iterator: _erase_it,    \
        key_type: _erase_k      \
    )(T, x)
iterator _erase_it(Rb_tree *T, iterator z);             // erase(Rb_tree, iterator) in O(logn), returns iterator to next element
insert_erase_t _erase_k(Rb_tree *T, key_type key);      // erase(Rb_tree, key) in O(logn), returns {success: 0/1, iterator to next element}


// Helper functions
void fix_insert(iterator *root, iterator z);
void fix_erase(Rb_tree *T, iterator x);
void left_rotate(iterator *root, iterator x);
void right_rotate(iterator *root, iterator y);
void transplant(iterator *root, iterator x, iterator y);

#endif  /* RB_TREE_H */
