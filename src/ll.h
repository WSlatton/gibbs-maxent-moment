#ifndef GIBBS_MAXENT_MOMENT_LL_H
#define GIBBS_MAXENT_MOMENT_LL_H

#include <stdlib.h>

typedef struct {
    struct node *root;
} LL;

struct node {
   char *elem;
   struct node *next;
};

LL *ll_new();

void ll_insert(LL *ll, char *x);

char *ll_next(LL *ll);

#endif //GIBBS_MAXENT_MOMENT_LL_H
