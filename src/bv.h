#ifndef GIBBS_MAXENT_MOMENT_BV_H
#define GIBBS_MAXENT_MOMENT_BV_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    size_t n;
    uint64_t *mem;
} BV;

BV *bv_create(size_t n);

void bv_set(BV *bv, size_t i, bool v);

bool bv_get(BV *bv, size_t i);

void bv_free(BV *bv);

#endif //GIBBS_MAXENT_MOMENT_BV_H
