#include "gibbs_maxent_moment.h"
#include "bv.h"
#include "sys/time.h"
#include <stdlib.h>
#include <math.h>

bool contains(int *sorted_list, size_t n, size_t x) {
    for (int i = 0; i < n; i++) {
        if (sorted_list[i] == x) {
            return true;
        } else if (sorted_list[i] > x) {
            return false;
        }
    }

    return false;
}

void compute_moment_slices(distribution *dist) {
    int N = dist->N;
    int P = dist->P;
    int K = dist->K;

    dist->moment_slices = (int **) malloc(sizeof(int *) * dist->N);
    dist->moment_slices_sizes = (size_t *) malloc(sizeof(size_t) * dist->N);

    for (int i = 0; i < N; i++) {
        int Pi = 0;

        for (int j = 0; j < P; j++) {
            if (contains(dist->moments + j * K, K, i)) {
                Pi++;
            }
        }

        dist->moment_slices[i] = malloc(sizeof(size_t) * Pi);
        dist->moment_slices_sizes[i] = Pi;

        int k = 0;
        for (int j = 0; j < P; j++) {
            if (contains(dist->moments + j * K, K, i)) {
                dist->moment_slices[i][k++] = j;
            }
        }
    }
}

void randomize_state(BV *state) {
    for (int i = 0; i < state->n; i++) {
        bv_set(state, i, rand() & 1);
    }
}

void seed_rand() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned seed = (unsigned) tv.tv_usec;
    srand(seed);
}

double rand_double() {
    uint64_t r53 = (((uint64_t) rand()) << 20) ^((uint64_t) rand());
    return (double) r53 / 2251799813685247.0;
}

bool subset(int *list, size_t n, BV *bv, int ignore) {
    for (int i = 0; i < n; i++) {
        if (list[i] == -1) {
            return true;
        }

        if (bv_get(bv, list[i]) == 0 && list[i] != ignore) {
            return false;
        }
    }

    return true;
}

void step(distribution *dist, BV *state) {
    size_t N = dist->N;
    size_t K = dist->K;

    for (int i = 0; i < N; i++) {
        int Pi = dist->moment_slices_sizes[i];
        double u = rand_double();
        double log_ratio = 0;

        for (int j = 0; j < Pi; j++) {
            size_t moment_index = dist->moment_slices[i][j];
            int *moment = dist->moments + moment_index * K;

            if (subset(moment, K, state, i)) {
                double c = dist->coefficients[moment_index];
                log_ratio += c;
            }
        }

        if (log_ratio <= log(1 - u) - log(u)) {
            bv_set(state, i, 0);
        } else {
            bv_set(state, i, 1);
        }
    }
}

void random_restart(distribution *dist, BV *state, size_t burn_in) {
    randomize_state(state);

    for (int i = 0; i < burn_in; i++) {
        step(dist, state);
    }
}

sparse_arr *bv_to_sparse(BV *bv) {
    size_t l = 0;

    for (int i = 0; i < bv->n; i++) {
        if (bv_get(bv, i)) {
            l++;
        }
    }

    int *array = (int *) malloc(sizeof(size_t) * l);
    size_t k = 0;

    for (int i = 0; i < bv->n; i++) {
        if (bv_get(bv, i)) {
            array[k++] = i;
        }
    }

    sparse_arr *sa = (sparse_arr *) malloc(sizeof(sparse_arr));
    sa->length = l;
    sa->array = array;

    return sa;
}

sparse_arr **sample(distribution *dist, size_t number_of_samples, size_t sample_interval, size_t burn_in) {
    size_t N = dist->N;
    seed_rand();
    compute_moment_slices(dist);
    BV *state = bv_create(N);
    random_restart(dist, state, burn_in);
    sparse_arr **samples = (sparse_arr **) malloc(sizeof(sparse_arr *) * number_of_samples);

    for (int s = 0; s < number_of_samples; s++) {
        for (int i = 0; i < sample_interval; i++) {
            step(dist, state);
        }

        samples[s] = bv_to_sparse(state);
    }

    bv_free(state);

    return samples;
}

void sparse_arr_free(sparse_arr *sa) {
    free(sa->array);
    free(sa);
}
