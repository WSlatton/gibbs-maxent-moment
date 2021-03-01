#include "bv.h"
#include <stdlib.h>
#include <stdio.h>

size_t div_round_up(size_t a, size_t b) {
    return (a + (b - 1)) / b;
}

BV *bv_create(size_t n) {
    size_t chunks = div_round_up(n, 64);
    BV *bv = (BV *) malloc(sizeof(bv));
    uint64_t *mem = (uint64_t *) malloc(sizeof(uint64_t) * chunks);
    bv->n = n;
    bv->mem = mem;
    return bv;
}

void bv_set(BV *bv, size_t i, bool v) {
    size_t chunk = i / 64;
    size_t offset = i % 64;
    bv->mem[chunk] = (bv->mem[chunk] & ~(1UL << offset)) | (v << offset);
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

BV *bv_copy(BV *bv) {
    BV *copy = (BV *) malloc(sizeof(BV));
    copy->n = bv->n;
    size_t chunks = div_round_up(bv->n, 64);
    copy->mem = (size_t *) malloc(sizeof(size_t) * chunks);

    for (int i = 0; i < chunks; i++) {
        copy->mem[i] = bv->mem[i];
    }

    return copy;
}

char *bv_to_str(BV *bv) {
    char *str = (char *) malloc(sizeof(char) * bv->n);

    for (int i = 0; i < bv->n; i++) {
        if (bv_get(bv, i)) {
            str[i] = '1';
        } else {
            str[i] = '0';
        }
    }

    return str;
}
