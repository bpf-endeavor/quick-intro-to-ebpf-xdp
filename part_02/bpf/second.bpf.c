#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <linux/if_ether.h> // ethhdr
#include <linux/ip.h> // iphdr
#include <linux/in.h> // IPPROTO_*
#include <linux/udp.h> // udphdr

__u64 counter;

SEC("xdp")
int xdp_main(struct xdp_md *ctx)
{
	bpf_printk("here");
	void *data = (void *)(__u64)ctx->data;
	void *data_end = (void *)(__u64)ctx->data_end;

	// look at Ethernet header
	struct ethhdr *eth = data;
	if ((eth + 1) > data_end) {
		// packet to small for ethernet
		return XDP_PASS;
	}
	// ethhdr definition:
	// struct ethhdr {
	// 	unsigned char	h_dest[6];	/* destination eth addr	*/
	// 	unsigned char	h_source[6];	/* source ether addr	*/
	// 	__be16		h_proto;	/* packet type ID field	*/
	// } __attribute__((packed));
	// Do you know what bpf_ntohs does? If not please ask :)
	if (eth->h_proto != bpf_ntohs(ETH_P_IP)) {
		// not an ip header
		return XDP_PASS;
	}

	// Look at IP header
	struct iphdr *ip = (eth + 1);
	if ((ip + 1) > data_end) {
		return XDP_PASS;
	}
	// iphdr definition:
	// struct iphdr {
	// 	__u8	ihl:4,
	// 		version:4;
	// 	__u8	tos;
	// 	__be16	tot_len;
	// 	__be16	id;
	// 	__be16	frag_off;
	// 	__u8	ttl;
	// 	__u8	protocol;
	// 	__be16	check;
	// 	__be32	saddr;
	// 	__be32	daddr;
	// 	/*The options start here. */
	// };
	if (ip->protocol != IPPROTO_UDP) {
		// not an UDP packet
		return XDP_PASS;
	}

	__u16 offset = ip->ihl * 4;
	// do you know why we first cast to __u8 and then add offset? Please
	// ask if not. it is important.
	struct udphdr *udp = (__u8 *)(ip) + offset;

	// Look at UDP header
	if ((udp + 1) > data_end) {
		return XDP_PASS;
	}
	// udphdr definition
	// struct udphdr {
	// 	__be16	source;
	// 	__be16	dest;
	// 	__be16	len;
	// 	__sum16	check;
	// };
	if (udp->dest != bpf_ntohs(8080)) {
		// did not matched the port
		return XDP_PASS;
	}

	// found a matching packet
	bpf_printk("matched!");
	counter++;
	return XDP_DROP;
}

char LICENSE[] SEC("license") = "GPL";
