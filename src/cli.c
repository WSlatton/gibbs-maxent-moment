#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "gibbs_maxent_moment.h"
#include "ll.h"

int *parse_moments(size_t *P, size_t* K) {
    size_t size = 0;
    char *line = NULL;
    LL *lines = ll_new();
    *P = 0;
    size_t n;
    *K = 0;

    while ((n = getline(&line, &size, stdin)) > 1) {
        char *line_copy = (char *) malloc(sizeof(char) * n);
        strcpy(line_copy, line);
        ll_insert(lines, line_copy);
        (*P)++;

        size_t K_line = 1;

        for (int i = 0; i < n; i++) {
            if (line[i] == ' ') {
                K_line++;
            }
        }

        if (K_line > *K) {
            *K = K_line;
        }
    }

    int *moments = (int *) malloc(sizeof(int) * (*P) * (*K));

    char *d = " ";

    for (size_t j = 0; j < *P; j++) {
        char *line = ll_next(lines);
        char *p = strtok(line, d);
        size_t i = 0;
        size_t k = 0;

        while (p != NULL) {
            sscanf(p, "%d", &i);
            moments[j * (*K) + k] = i;
            k++;
            p = strtok(NULL, d);
        }

        for (; k < *K; k++) {
            moments[j * (*K) + k] = -1;
        }
    }

    return moments;
}

double *parse_coefficients(int P) {
    double *coefficients = (double *) malloc(sizeof(double) * P);

    for (int j = 0; j < P; j++) {
        scanf("%lf", &coefficients[j]);
    }

    return coefficients;
}

int main(int argc, char **argv) {
    size_t N;
    size_t number_of_samples;
    size_t sample_interval;
    size_t burn_in;

    sscanf(argv[1], "%zu", &N);
    sscanf(argv[2], "%zu", &number_of_samples);
    sscanf(argv[3], "%zu", &sample_interval);
    sscanf(argv[4], "%zu", &burn_in);

    size_t P;
    size_t K;
    int *moments = parse_moments(&P, &K);
    double *coefficients = parse_coefficients(P);

    distribution dist = {
            .N = N,
            .P = P,
            .K = K,
            .moments = moments,
            .coefficients = coefficients
    };

    sparse_arr **samples = sample(&dist, number_of_samples, sample_interval, burn_in);

    for (int s = 0; s < number_of_samples; s++) {
        int l = samples[s]->length;
        for (int i = 0; i < l - 1; i++) {
            printf("%i ", samples[s]->array[i]);
        }

        printf("%i\n", samples[s]->array[l - 1]);
    }
}
