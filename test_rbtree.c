#include <stdio.h>
#include <assert.h>

// #define KEY_TYPE int       // any type may be used, int by default
// #define VALUE_TYPE int
#include "RBtree.h"

/* Provide a compare function if default key type int is not used
int comp_keys(const key_type A, const key_type B) {
    return A == B? 0 : A < B? -1 : +1;
} */

int main() {
    int n = 6;
    int a[] = {10, 5, 6, 6, -12, 30};
    printf("Array:\n");
    printf("%d\n", (int)sizeof(a)/sizeof *a);
    for(int i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n\n");
    
    // Set operations
    Rb_tree set = newrbtree();
    for(int i = 0; i < n; i++)
        insert(&set, a[i]);       // value is optional

    printf("Set:\n");
    printf("%d\n", set.size);
    for(iterator it = set.begin; it != T_nil; it = next(it)) {        // forward iteration
        printf("%d ", it->key);
    } printf("\n");

    if(contains(&set, 30)) {
        printf("Set contains %d\n", 30);
    } else {
        printf("Set does not contain %d\n", 30);
    }
    if(contains(&set, 42)) {
        printf("Set contains %d\n", 42);
    } else {
        printf("Set does not contain %d\n", 42);
    }

    printf("\n");
    printf("lower_bound(%d) = %d\n", 5, lower_bound(&set, 5)->key);
    printf("upper_bound(%d) = %d\n", 5, upper_bound(&set, 5)->key);
    printf("lower_bound(%d) = %d\n", 17, lower_bound(&set, 17)->key);
    printf("upper_bound(%d) = %d\n", 17, upper_bound(&set, 17)->key);

    iterator it = find(&set, 6);
    erase(&set, it);      // erase using iterator
    erase(&set, 5);       // erase by value
    printf("\nSet after erase(%d) and erase(%d):\n", 5, 6);
    printf("%d\n", set.size);
    for(iterator it = set.begin; it != T_nil; it = next(it)) {        // forward iteration
        printf("%d ", it->key);
    } printf("\n");

    Rb_tree map = newrbtree();
    for(int i = 0; i < n; i += 2)
        insert(&map, a[i], a[i+1]);

    printf("\n\nMap:\n");
    printf("%d\n", map.size);
    for(iterator it = map.begin; it != T_nil; it = next(it)) {      // forward iteration
        printf("(%d, %d) ", it->key, it->val);
    } printf("\n");

    printf("\nmap[6]++, map[-12] = -42 and map.insert(100, 1)\n");
    find(&map, 6)->val++;
    find(&map, -12)->val = -42;
    insert(&map, 100, 1);

    printf("\nIn reverse order:\n");
    printf("%d\n", map.size);
    for(iterator it = map.rbegin; it != T_nil; it = prev(it)) {      // reverse iteration
        printf("(%d, %d) ", it->key, it->val);
    } printf("\n");
}