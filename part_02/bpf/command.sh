clang -S \
	-target bpf \
	-g -O2 -emit-llvm \
	-o ./second.bpf.ll second.bpf.c
llc -mcpu=probe -march=bpf -filetype=obj -o second.bpf.o second.bpf.ll
bpftool gen skeleton ./second.bpf.o name second > second.skel.h
