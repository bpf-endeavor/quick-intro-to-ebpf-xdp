#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("xdp")
int xdp_main(struct xdp_md *ctx)
{
	bpf_printk("here");
	return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
