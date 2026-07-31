// ==============================================================================
// Subproject 05: stress-chaos-test (Red Team - Chaos Injector)
// Agent 10: CPU Cache Thrash & Context-Switch Saturator
// Saturatore di L1/L2/L3 Cache e generatore di Context Switch Storm
// ==============================================================================

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define NUM_THREADS 16
#define CACHE_SIZE_MB 64
#define ARRAY_SIZE (CACHE_SIZE_MB * 1024 * 1024 / sizeof(int))

static volatile int stop_stress = 0;

void *cache_thrash_worker(void *arg) {
    int thread_id = *(int *)arg;
    int *large_array = malloc(ARRAY_SIZE * sizeof(int));
    if (!large_array) {
        perror("malloc fallita");
        pthread_exit(NULL);
    }

    // Inizializzazione array
    for (size_t i = 0; i < ARRAY_SIZE; i += 16) {
        large_array[i] = i ^ thread_id;
    }

    unsigned long long access_count = 0;
    size_t stride = 16; // 64 Byte stride (pari alla dimensione di una Cache Line)

    while (!stop_stress) {
        // Cache Pollution Loop: Accesso non sequenziale per forzare Cache Eviction in L1/L2/L3
        for (size_t i = 0; i < ARRAY_SIZE; i += stride) {
            large_array[i] += thread_id + (int)i;
        }

        access_count += ARRAY_SIZE / stride;

        // Provoca intenzionalmente context-switch aggressivi
        if ((access_count & 0xFFFF) == 0) {
            sched_yield();
        }
    }

    free(large_array);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    printf("==================================================================\n");
    printf("💥 [Agent 10 - Red Team] CPU Cache Thrash & Context-Switch Saturator\n");
    printf("==================================================================\n");

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    printf("⚡ [Agent 10] Spawning %d threads per inquinamento L1/L2/L3 Cache (Array Size: %d MB)...\n",
           NUM_THREADS, CACHE_SIZE_MB);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, cache_thrash_worker, &thread_ids[i]) != 0) {
            perror("pthread_create fallita");
            return 1;
        }
    }

    printf("🔥 [Agent 10] Inquinamento Cache & Storm di Context-Switch avviati!\n");
    printf("⏳ Esecuzione stress test in corso per 15 secondi...\n");

    sleep(15);
    stop_stress = 1;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("👋 [Agent 10] Stress test di inquinamento Cache completato.\n");
    return 0;
}
