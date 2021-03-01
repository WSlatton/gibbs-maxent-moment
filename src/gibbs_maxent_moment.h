#ifndef GIBBS_MAXENT_MOMENT_GIBBS_MAXENT_MOMENT_H
#define GIBBS_MAXENT_MOMENT_GIBBS_MAXENT_MOMENT_H

#include "bv.h"
#include <stddef.h>

typedef struct {
    size_t N;
    size_t P;
    size_t K;
    int *moments;
    int **moment_slices;
    size_t *moment_slices_sizes;
    double *coefficients;
} distribution;

typedef struct {
    size_t length;
    int *array;
} sparse_arr;

sparse_arr **sample(distribution *dist, size_t number_of_samples, size_t sample_interval, size_t burn_in);

#endif //GIBBS_MAXENT_MOMENT_GIBBS_MAXENT_MOMENT_H
