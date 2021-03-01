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
    ll->root = ll->root->next;
    return elem;
}

void ll_free(LL *ll) {
    if (ll->root == NULL) {
        return;
    }

    free(ll->root->elem);
    ll->root = ll->root->next;
    ll_free(ll);
}
