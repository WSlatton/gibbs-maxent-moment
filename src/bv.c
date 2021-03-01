#include "bv.h"
#include <stdlib.h>

size_t div_round_up(size_t a, size_t b) {
    return (a + (b - 1)) / b;
}

BV *bv_create(size_t n) {
    BV *bv = (BV *) malloc(sizeof(BV));
    int chunks = div_round_up(n, 64);
    bv->mem = (uint64_t *) malloc(sizeof(uint64_t) * chunks);
    bv->n = n;
    return bv;
}

void bv_set(BV *bv, size_t i, bool v) {
    size_t chunk = i / 64;
    size_t offset = i % 64;
    bv->mem[chunk] = (bv->mem[chunk] & ~(1UL << offset)) | ((uint64_t) v << offset);
}

bool bv_get(BV *bv, size_t i) {
    size_t chunk = i / 64;
    size_t offset = i % 64;
    return (bv->mem[chunk] >> offset) & 1;
}

void bv_free(BV *bv) {
    free(bv->mem);
    free(bv);
}
