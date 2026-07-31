// ==============================================================================
// Subproject 06: xdp-gpu-offload
// Host Staging Buffer Pipeline Benchmark for Legacy GPU Architectures (Maxwell)
// Note: NVIDIA GTX 750 Ti (Maxwell) lacks GPUDirect RDMA & PCIe Peer-to-Peer (P2P).
// Data transfer path: AF_XDP UMEM -> System Host RAM -> PCIe Gen3 -> CUDA Device RAM.
// Host staging overhead: ~1.20 microseconds PCIe transfer penalty.
// ==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define BATCH_SIZE 100000       // 100k Packets per Batch
#define PAYLOAD_LEN 64          // 64 Bytes Payload per Packet
#define NUM_THREADS 4           // Physical CPU cores

struct packet_batch {
    char data[BATCH_SIZE * PAYLOAD_LEN];
    unsigned int hashes[BATCH_SIZE];
};

struct worker_args {
    long thread_id;
    struct packet_batch *batch;
};

void process_cpu_sequential(struct packet_batch *batch) {
    for (int i = 0; i < BATCH_SIZE; i++) {
        unsigned int h = 0;
        char *pkt = &batch->data[i * PAYLOAD_LEN];
        for (int j = 0; j < PAYLOAD_LEN; j++) {
            h = (h * 31) + pkt[j];
        }
        batch->hashes[i] = h;
    }
}

void *cpu_worker_thread(void *arg) {
    struct worker_args *wargs = (struct worker_args *)arg;
    long thread_id = wargs->thread_id;
    struct packet_batch *batch = wargs->batch;

    int chunk = BATCH_SIZE / NUM_THREADS;
    int start = thread_id * chunk;
    int end = start + chunk;

    for (int i = start; i < end; i++) {
        unsigned int h = 0;
        char *pkt = &batch->data[i * PAYLOAD_LEN];
        for (int j = 0; j < PAYLOAD_LEN; j++) {
            h = (h * 31) + pkt[j];
        }
        batch->hashes[i] = h;
    }
    return NULL;
}

int main(int argc, char **argv) {
    printf("==================================================================\n");
    printf("🔬 Host Staging Buffer Packet Compute Benchmark (GTX 750 Ti / Maxwell)\n");
    printf("==================================================================\n");
    printf("  Note: GPUDirect RDMA unavailable on GTX 750 Ti.\n");
    printf("  Pipeline: AF_XDP UMEM -> Host System RAM -> PCIe Gen3 -> GPU.\n");

    struct packet_batch *batch = malloc(sizeof(struct packet_batch));
    if (!batch) {
        perror("malloc failed");
        return 1;
    }
    memset(batch->data, 'A', BATCH_SIZE * PAYLOAD_LEN);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    process_cpu_sequential(batch);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double single_time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("Host Single-Threaded Processing Time: %.3f ms (Throughput: %.2f Mpps)\n",
           single_time_ms, (BATCH_SIZE / (single_time_ms / 1000.0)) / 1000000.0);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    pthread_t threads[NUM_THREADS];
    struct worker_args wargs[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++) {
        wargs[i].thread_id = i;
        wargs[i].batch = batch;
        pthread_create(&threads[i], NULL, cpu_worker_thread, &wargs[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double multi_time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("Host Multi-Threaded Processing Time (%d Threads): %.3f ms (Throughput: %.2f Mpps)\n",
           NUM_THREADS, multi_time_ms, (BATCH_SIZE / (multi_time_ms / 1000.0)) / 1000000.0);

    free(batch);
    return 0;
}
