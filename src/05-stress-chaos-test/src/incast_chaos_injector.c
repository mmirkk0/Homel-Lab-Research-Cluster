/*
 * Incast Microburst & Tail Latency Evaluator
 * Simulates network incast congestion microbursts and measures p99.9 and p99.99 tail latencies.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_SAMPLES 10000

int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

int main(int argc, char **argv) {
    printf("==================================================================\n");
    printf("🔬 Incast Microburst & Tail Latency Evaluator (p99.9 / p99.99)\n");
    printf("==================================================================\n");

    double *latencies = malloc(TOTAL_SAMPLES * sizeof(double));
    if (!latencies) {
        perror("malloc failed");
        return 1;
    }

    struct timespec start, end;

    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        // Inject artificial microburst congestion every 1000 iterations
        if (i % 1000 == 0) {
            volatile int spin = 0;
            for (int k = 0; k < 50000; k++) { spin += k; }
        } else {
            volatile int spin = 0;
            for (int k = 0; k < 500; k++) { spin += k; }
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        double elapsed_us = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_nsec - start.tv_nsec) / 1000.0;
        latencies[i] = elapsed_us;
    }

    qsort(latencies, TOTAL_SAMPLES, sizeof(double), compare_doubles);

    printf("\nEmpirical Microburst Tail Latency Results (%d Samples):\n", TOTAL_SAMPLES);
    printf("  Median Latency (p50):       %.3f us\n", latencies[(int)(TOTAL_SAMPLES * 0.50)]);
    printf("  90th Percentile (p90):      %.3f us\n", latencies[(int)(TOTAL_SAMPLES * 0.90)]);
    printf("  99th Percentile (p99):      %.3f us\n", latencies[(int)(TOTAL_SAMPLES * 0.99)]);
    printf("  99.9th Percentile (p99.9):  %.3f us\n", latencies[(int)(TOTAL_SAMPLES * 0.999)]);
    printf("  99.99th Percentile (p99.99):%.3f us\n", latencies[(int)(TOTAL_SAMPLES * 0.9999)]);

    free(latencies);
    return 0;
}
