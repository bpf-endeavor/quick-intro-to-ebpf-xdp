#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <net/if.h>
#include <signal.h>
#include <linux/if_link.h>
#include <bpf/bpf.h>

#include "../bpf/kprobe.skel.h"

static volatile int running = 0;

static void handle_signal(int s)
{
	running = 0;
}

int main(int argc, char *argv[])
{
	struct kprobe *skel = kprobe__open_and_load();
	if (skel == NULL) {
		fprintf(stderr, "Failed to open and load the program\n");
		return EXIT_FAILURE;
	}
	kprobe__attach(skel);
	running = 1;
	signal(SIGINT, handle_signal);
	signal(SIGHUP, handle_signal);
	printf("Hit Ctrl-C ...\n");
	while(running) {pause();}
	kprobe__detach(skel);
	kprobe__destroy(skel);
	printf("done\n");
	return 0;
}
