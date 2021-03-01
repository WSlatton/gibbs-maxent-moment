#include "ll.h"

LL *ll_new() {
    LL *ll = (LL *) malloc(sizeof(LL));
    ll->root = NULL;
    return ll;
}

void ll_insert(LL *ll, char *elem) {
    struct node *node = ll->root;

    if (node == NULL) {
        ll->root = (struct node *) malloc(sizeof(struct node));
        ll->root->elem = elem;
        ll->root->next = NULL;
        return;
    }

    while (node->next != NULL) {
        node = node->next;
    }

    node->next = (struct node *) malloc(sizeof(struct node));
    node->next->elem = elem;
    node->next->next = NULL;
}

char *ll_next(LL *ll) {
    char *elem = ll->root->elem;
    struct node *old_root = ll->root;
    ll->root = ll->root->next;
    free(old_root);
    return elem;
}
