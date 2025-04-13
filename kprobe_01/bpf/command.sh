clang -S \
	-target bpf \
	-g -O2 -emit-llvm \
	-o ./kprobe.bpf.ll kprobe.bpf.c
llc -mcpu=probe -march=bpf -filetype=obj -o kprobe.bpf.o kprobe.bpf.ll
bpftool gen skeleton ./kprobe.bpf.o name kprobe > kprobe.skel.h
