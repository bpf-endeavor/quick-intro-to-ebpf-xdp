#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <linux/if_ether.h> // ethhdr
#include <linux/ip.h> // iphdr
#include <linux/in.h> // IPPROTO_*
#include <linux/udp.h> // udphdr

__u64 counter;

#include "types.h"
#include "xdp_helpers.h"

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__type(key, struct key);
	__type(value, struct value);
	__uint(max_entries, 16);
} table SEC(".maps");

SEC("xdp")
int xdp_main(struct xdp_md *ctx)
{
	int ret;
	bpf_printk("here");
	void *data = (void *)(__u64)ctx->data;
	void *data_end = (void *)(__u64)ctx->data_end;

	// look at Ethernet header
	struct ethhdr *eth = data;
	if ((eth + 1) > data_end) {
		// packet to small for ethernet
		return XDP_PASS;
	}
	if (eth->h_proto != bpf_ntohs(ETH_P_IP)) {
		// not an ip header
		return XDP_PASS;
	}

	// Look at IP header
	struct iphdr *ip = (eth + 1);
	if ((ip + 1) > data_end) {
		return XDP_PASS;
	}
	if (ip->protocol != IPPROTO_UDP) {
		// not an UDP packet
		return XDP_PASS;
	}

	__u16 offset = ip->ihl * 4;
	struct udphdr *udp = (__u8 *)(ip) + offset;
	if ((udp + 1) > data_end) {
		return XDP_PASS;
	}
	if (udp->dest != bpf_ntohs(8080)) {
		// did not matched the port
		return XDP_PASS;
	}

	char *query = (udp+1);
	int len = 0;
	int i;
	struct key K = {};
	for (i = 0; i < MAX_STR_SIZE; i++) {
		if ((query + i + 1) > data_end) {
			break;
		}
		K.data[i] = query[i];
	}
	len = i;
	if (len == 0 || len >= MAX_STR_SIZE) {
		bpf_printk("request too large");
		return XDP_DROP;
	}

	struct value *V;
	V = bpf_map_lookup_elem(&table, &K);

	if (V == NULL) {
		bpf_printk("object not found for key %s", K.data);
		return XDP_DROP;
	}

	// found a matching packet
	bpf_printk("matched! %s", V->data);
	counter++;

	// rewrite the packet and send it back
	int resp_len = 0;
	for (i = 0; i < MAX_STR_SIZE; i++) {
		if (V->data[i] == '\0') {
			break;
		}
	}
	resp_len = i;

	int delta = resp_len - len;
	/* bpf_printk("delta: %d", delta); */
	ret = bpf_xdp_adjust_tail(ctx, delta);
	if (ret < 0) {
		bpf_printk("failed to resize");
		return XDP_DROP;
	}

	data = (void *)(__u64)ctx->data;
	data_end = (void *)(__u64)ctx->data_end;
	char *ptr = data + 14 + 20 + 8;
	for (i = 0; i < MAX_STR_SIZE; i++) {
		if (i >= resp_len) {
			break;
		}
		if (&ptr[i + 1] > data_end) {
			bpf_printk("something wrong! %d/%d", i, resp_len);
			return XDP_DROP;
		}
		ptr[i] = V->data[i];
	}

	// update checksum
	__prepare_headers_before_send(ctx);

	return XDP_TX;
}

char LICENSE[] SEC("license") = "GPL";
