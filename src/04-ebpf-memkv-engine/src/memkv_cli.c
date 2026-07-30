// ==============================================================================
// Subproject 04: ebpf-memkv-engine
// Agent 4B: Zero-Copy Storage Engine Lead
// User-Space CLI Client per l'ispezione della memoria BPF (Linux 7.1.5 Mainline)
// ==============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

struct kv_entry {
    __u32 key_id;
    __u64 value_data;
    __u64 access_counter;
};

int main(int argc, char **argv) {
    printf("==================================================================\n");
    printf("🚀 [Agent 4A/4B] eBPF In-Memory Key-Value Storage CLI Client\n");
    printf("==================================================================\n");

    struct bpf_object *obj = bpf_object__open_file("memkv_kernel.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "❌ Errore apertura memkv_kernel.o\n");
        return 1;
    }

    if (bpf_object__load(obj)) {
        fprintf(stderr, "❌ Errore caricamento mappa BPF nel kernel\n");
        return 1;
    }

    int map_fd = bpf_object__find_map_fd_by_name(obj, "memkv_map");
    if (map_fd < 0) {
        fprintf(stderr, "❌ Mappa BPF memkv_map non trovata nel kernel\n");
        return 1;
    }

    // Inserimento Chiave/Valore Test nel Kernel
    __u32 key = 42;
    struct kv_entry entry = {
        .key_id = 42,
        .value_data = 0xDEADBEEFCAFEULL,
        .access_counter = 1
    };

    if (bpf_map_update_elem(map_fd, &key, &entry, BPF_ANY) == 0) {
        printf("✅ Inserita Chiave %u con Valore 0x%llX direttamente nella RAM Kernel!\n",
               key, (unsigned long long)entry.value_data);
    }

    // Lettura Immediata
    struct kv_entry read_entry;
    if (bpf_map_lookup_elem(map_fd, &key, &read_entry) == 0) {
        printf("🔍 Lettura BPF Map Kernel: Key=%u, Value=0x%llX, Accessi=%llu\n",
               read_entry.key_id, (unsigned long long)read_entry.value_data,
               (unsigned long long)read_entry.access_counter);
    }

    bpf_object__close(obj);
    printf("👋 Operazione completata.\n");
    return 0;
}
