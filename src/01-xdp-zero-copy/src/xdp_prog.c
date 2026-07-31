/*
 * AF_XDP Zero-Copy Driver Hook with BPF Tail Calls
 * Performs L2/L3 header parsing zero-copy and redirects to AF_XDP socket (XSKMAP).
 */

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <bpf/bpf_helpers.h>

struct {
    __uint(type, BPF_MAP_TYPE_XSKMAP);
    __uint(max_entries, 64);
    __uint(key_size, sizeof(int));
    __uint(value_size, sizeof(int));
} xsks_map SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_PROG_ARRAY);
    __uint(max_entries, 8);
    __uint(key_size, sizeof(int));
    __uint(value_size, sizeof(int));
} jmp_table SEC(".maps");

SEC("xdp")
int xdp_parse_l2(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    if (eth->h_proto == __builtin_bswap16(ETH_P_IP)) {
        // Tail call into L3 parser stage (Index 0 in jmp_table)
        bpf_tail_call(ctx, &jmp_table, 0);
    }

    // Default zero-copy redirect to AF_XDP socket 0
    return bpf_redirect_map(&xsks_map, ctx->rx_queue_index, XDP_PASS);
}

SEC("xdp_l3_parser")
int xdp_parse_l3(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data     = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    struct iphdr *iph  = (void *)(eth + 1);
    if ((void *)(iph + 1) > data_end)
        return XDP_PASS;

    // Fast-path redirection to AF_XDP socket
    return bpf_redirect_map(&xsks_map, ctx->rx_queue_index, XDP_PASS);
}

char _license[] SEC("license") = "GPL";
