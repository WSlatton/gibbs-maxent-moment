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
    char *separator = "%%\n";

    while (1) {
        size_t n = getline(&line, &size, stdin);

        if (strcmp(line, separator) == 0) {
          break;
        } else if (n == -1) {
          fprintf(stderr, "No separator line in input. Failed to parse.\n");
          exit(1);
        }

        char *line_copy = (char *) malloc(sizeof(char) * (n + 1));
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

    free(line);

    int *moments = (int *) malloc(sizeof(int) * (*P) * (*K));

    char *d = " ";

    for (size_t j = 0; j < *P; j++) {
        char *line = ll_next(lines);
        char *p = strtok(line, d);
        int i = 0;
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

        free(line);
    }

    free(lines);

    return moments;
}

double *parse_coefficients(int P) {
    double *coefficients = (double *) malloc(sizeof(double) * P);

    for (int j = 0; j < P; j++) {
        scanf("%lf", &coefficients[j]);
    }

    return coefficients;
}

void verify_dist(distribution *dist) {
  // check that moments are sorted, which is assumed by sampling code
  for (int i = 0; i < dist->P; i++) {
    int last_index = -1;
    bool hit_end = false;
    for (int j = 0; j < dist->K; j++) {
      int index = dist->moments[i * dist->K + j];

      if (index < -2 || index >= (int) dist->N) {
        fprintf(stderr, "Invalid index %i when N = %zu\n", index, dist->N);
        exit(1);
      }
      
      if (index == -1) {
        hit_end = true;
      } else if (hit_end && index != -1) {
        fprintf(stderr, "Failed to parse moments.\n");
        exit(1);
      } else if (index <= last_index) {
        fprintf(stderr, "Indices in a moment were not sorted.\n");
        exit(1);
      }

      last_index = index;
    }
  }
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

    verify_dist(&dist);

    sparse_arr **samples = sample(&dist, number_of_samples, sample_interval, burn_in);

    for (int s = 0; s < number_of_samples; s++) {
        int l = samples[s]->length;
        for (int i = 0; i < l - 1; i++) {
            printf("%i ", samples[s]->array[i]);
        }

        if (l == 0) {
          printf("\n");
        } else {
          printf("%i\n", samples[s]->array[l - 1]);
        }
        sparse_arr_free(samples[s]);
    }

    /*printf("moments:\n");
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < K; j++) {
            printf("%d ", moments[i * K + j]);
        }
        printf(" (%.2f)\n", coefficients[i]);
    }

    for (int i = 0; i < N; i++) {
        printf("slices for i = %d:\n", i);
        int Pi = dist.moment_slices_sizes[i];
        int *slice = dist.moment_slices[i];

        for (int j = 0; j < Pi; j++) {
            printf("%zu ", slice[j]);
        }

        printf("\n");
    }*/

    free(moments);

    for (int i = 0; i < N; i++) {
        free(dist.moment_slices[i]);
    }

    free(dist.moment_slices);
    free(dist.moment_slices_sizes);
    free(coefficients);
    free(samples);
}
