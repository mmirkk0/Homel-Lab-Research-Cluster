// ==============================================================================
// Subproject 01: xdp-zero-copy
// Agent 1A: XDP Driver & eBPF Kernel Developer
// Kernel eBPF XDP Packet Filter & Collector (Linux 7.1.5 Mainline)
// ==============================================================================

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>

struct pkt_stats {
    __u64 rx_packets;
    __u64 rx_bytes;
};

// BPF Map per statistiche pacchetti ad alte prestazioni per CPU
struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __type(key, __u32);
    __type(value, struct pkt_stats);
    __uint(max_entries, 1);
} rxe_pkt_stats SEC(".maps");

SEC("xdp")
int xdp_zero_copy_filter(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    // Bounds check per Ethernet Header
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    // Processa solo IPv4
    if (eth->h_proto != __builtin_bswap16(ETH_P_IP))
        return XDP_PASS;

    // Bounds check per IP Header
    struct iphdr *iph = (void *)(eth + 1);
    if ((void *)(iph + 1) > data_end)
        return XDP_PASS;

    __u32 key = 0;
    struct pkt_stats *stats = bpf_map_lookup_elem(&rxe_pkt_stats, &key);
    if (stats) {
        stats->rx_packets++;
        stats->rx_bytes += ((char *)data_end - (char *)data);
    }

    // Passa il pacchetto allo stack o reindirizza
    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
