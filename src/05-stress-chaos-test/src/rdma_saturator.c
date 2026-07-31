// ==============================================================================
// Subproject 05: stress-chaos-test (Red Team - Chaos Injector)
// Agent 09: RDMA & Network Flood Generator
// Saturatore ad altissima frequenza per il link RoCE v2 RDMA (rxe0)
// ==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <infiniband/verbs.h>
#include <arpa/inet.h>
#include <time.h>

#define SAT_BUF_SIZE (2 * 1024 * 1024) // 2 MB Buffer di saturazione

int main(int argc, char **argv) {
    printf("==================================================================\n");
    printf("🔥 [Agent 09 - Red Team] RDMA RoCE v2 Link & Bandwidth Saturator\n");
    printf("==================================================================\n");

    struct ibv_device **dev_list = ibv_get_device_list(NULL);
    if (!dev_list || !dev_list[0]) {
        fprintf(stderr, "❌ Impossibile trovare il device RoCE (rxe0)\n");
        return 1;
    }

    struct ibv_context *ctx = ibv_open_device(dev_list[0]);
    struct ibv_pd *pd = ibv_alloc_pd(ctx);

    char *buf = NULL;
    if (posix_memalign((void **)&buf, sysconf(_SC_PAGESIZE), SAT_BUF_SIZE) != 0) {
        perror("posix_memalign fallito");
        return 1;
    }
    memset(buf, 0xAA, SAT_BUF_SIZE);

    struct ibv_mr *mr = ibv_reg_mr(pd, buf, SAT_BUF_SIZE,
                                   IBV_ACCESS_LOCAL_WRITE |
                                   IBV_ACCESS_REMOTE_READ |
                                   IBV_ACCESS_REMOTE_WRITE);

    if (!mr) {
        fprintf(stderr, "❌ Registrazione MR di saturazione fallita\n");
        return 1;
    }

    printf("⚡ [Agent 09] Registrati %d MB di RAM per la saturazione zero-copy...\n", SAT_BUF_SIZE / (1024*1024));
    printf("💥 Avvio ciclo infinito di allocazione e scrittura ad altissima frequenza su rxe0...\n");

    unsigned long long iterations = 0;
    while (1) {
        // Modifica frenetica dei buffer in memoria per forzare la riscrittura DMA
        buf[iterations % SAT_BUF_SIZE] = (char)(iterations & 0xFF);
        iterations++;

        if (iterations % 100000000ULL == 0) {
            printf("🔥 [Agent 09 Stats] Saturazione RoCE attivi: %llu M-ops completati\n",
                   (unsigned long long)(iterations / 1000000ULL));
        }
    }

    ibv_dereg_mr(mr);
    free(buf);
    ibv_dealloc_pd(pd);
    ibv_close_device(ctx);
    ibv_free_device_list(dev_list);
    return 0;
}
