#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <linux/in.h>

struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, int);
    __type(value, int);
} port_map SEC(".maps");

#ifndef bpf_htons
#define bpf_htons(x) __builtin_bswap16(x)
#endif

SEC("xdp_drop_tcp")
int xdp_drop(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth = data;
    struct iphdr *ip;
    struct tcphdr *tcp;
    int nh_off = sizeof(*eth);
    int key = 0;
    int *port;

    port = bpf_map_lookup_elem(&port_map, &key);
    if (!port)
        return XDP_PASS;

    if (data + nh_off > data_end)
        return XDP_PASS;

    ip = data + nh_off;
    nh_off += sizeof(*ip);

    if (data + nh_off > data_end)
        return XDP_PASS;

    if (ip->protocol != IPPROTO_TCP)
        return XDP_PASS;

    tcp = (struct tcphdr *)(ip + 1);

    if ((void *)(tcp + 1) > data_end)
        return XDP_PASS;

    if (tcp->dest == bpf_htons(*port))
        return XDP_DROP;

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
