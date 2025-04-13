#define __TARGET_ARCH_x86 1

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <linux/ptrace.h>
#include <string.h>

SEC("kprobe/do_sys_openat2")
int kprobe_main(struct pt_regs *ctx)
{
	char *filename = (void *)PT_REGS_PARM2(ctx);
	char name[32];
	int len = bpf_probe_read_user_str(name, 32, filename);
	if (strncmp(name, "/home/hawk/safe_folder/", 23) == 0)
		bpf_printk("here %s", filename);
	return 0;
}

char LICENSE[] SEC("license") = "GPL";
