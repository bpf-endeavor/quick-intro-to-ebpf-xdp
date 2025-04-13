#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <net/if.h>
#include <signal.h>
#include <linux/if_link.h>
#include <bpf/bpf.h>

#include "../bpf/first.skel.h"

static char *ifacename = "veth1";
static int ifindex = -1;
static int xdp_flag = 0;
static volatile int running = 0;

static void handle_signal(int s)
{
	running = 0;
}

int main(int argc, char *argv[])
{
	ifindex = if_nametoindex(ifacename);
	if (!ifindex) {
		fprintf(stderr, "interface not found\n");
		return EXIT_FAILURE;
	}

	int ret;
	struct first *skel = first__open_and_load();
	if (skel == NULL) {
		fprintf(stderr, "Failed to open and load the program\n");
		return EXIT_FAILURE;
	}

	int prog_fd = bpf_program__fd(skel->progs.xdp_main);
	ret = bpf_xdp_attach(ifindex, prog_fd, xdp_flag, NULL);
	if (ret != 0) {
		fprintf(stderr, "Failed to attach the program\n");
		bpf_xdp_detach(ifindex, xdp_flag, NULL);
		return EXIT_FAILURE;
	}
	running = 1;
	signal(SIGINT, handle_signal);
	signal(SIGHUP, handle_signal);
	printf("Hit Ctrl-C ...\n");
	while(running) {pause();}
	bpf_xdp_detach(ifindex, xdp_flag, NULL);
	printf("done\n");
	return 0;
}
