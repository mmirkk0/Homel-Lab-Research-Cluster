// ==============================================================================
// Subproject 01: xdp-zero-copy
// Agent 1B: AF_XDP Ring Buffer Engineer
// User-Space Loader & BPF Map Monitor (Linux 7.1.5 Mainline)
// ==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <linux/if_link.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

static volatile int keep_running = 1;

void sig_handler(int sig) {
    keep_running = 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <interfaccia_rete>\n", argv[0]);
        fprintf(stderr, "Esempio: %s enp12s0\n", argv[0]);
        return 1;
    }

    const char *ifname = argv[1];
    int ifindex = if_nametoindex(ifname);
    if (!ifindex) {
        perror("if_nametoindex fallito");
        return 1;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    printf("🚀 [Agent 1A/1B] Caricamento programma eBPF XDP su %s (ifindex %d)...\n", ifname, ifindex);

    struct bpf_object *obj = bpf_object__open_file("xdp_prog.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "❌ Errore nell'apertura del file eBPF object xdp_prog.o\n");
        return 1;
    }

    if (bpf_object__load(obj)) {
        fprintf(stderr, "❌ Errore nel caricamento dell'oggetto eBPF nel kernel\n");
        return 1;
    }

    struct bpf_program *prog = bpf_object__find_program_by_name(obj, "xdp_zero_copy_filter");
    if (!prog) {
        fprintf(stderr, "❌ Programma xdp_zero_copy_filter non trovato\n");
        return 1;
    }

    int prog_fd = bpf_program__fd(prog);
    if (prog_fd < 0) {
        fprintf(stderr, "❌ Descrittore programma eBPF non valido\n");
        return 1;
    }

    // Attach al driver XDP
    int err = bpf_xdp_attach(ifindex, prog_fd, XDP_FLAGS_SKB_MODE, NULL);
    if (err) {
        fprintf(stderr, "❌ Impossibile collegare il programma XDP all'interfaccia %s\n", ifname);
        return 1;
    }

    printf("✅ Programma XDP caricato ed attivo su %s!\n", ifname);
    printf("📊 Monitoraggio pacchetti in corso (Premi Ctrl+C per uscire)...\n");

    int map_fd = bpf_object__find_map_fd_by_name(obj, "rxe_pkt_stats");
    while (keep_running) {
        sleep(1);
        if (map_fd >= 0) {
            __u32 key = 0;
            int num_cpus = libbpf_num_possible_cpus();
            struct {
                __u64 rx_packets;
                __u64 rx_bytes;
            } stats[num_cpus];

            if (bpf_map_lookup_elem(map_fd, &key, stats) == 0) {
                __u64 total_pkts = 0;
                __u64 total_bytes = 0;
                for (int i = 0; i < num_cpus; i++) {
                    total_pkts += stats[i].rx_packets;
                    total_bytes += stats[i].rx_bytes;
                }
                printf("\r📈 [XDP Metrics] Pacchetti Totali: %llu | Byte Totali: %llu B",
                       (unsigned long long)total_pkts, (unsigned long long)total_bytes);
                fflush(stdout);
            }
        }
    }

    printf("\n🧹 Stacco programma XDP da %s e pulizia risorse...\n", ifname);
    bpf_xdp_detach(ifindex, XDP_FLAGS_SKB_MODE, NULL);
    bpf_object__close(obj);
    printf("👋 Completato.\n");
    return 0;
}
