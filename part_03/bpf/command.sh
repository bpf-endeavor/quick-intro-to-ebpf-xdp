clang -S \
	-target bpf \
	-g -O2 -emit-llvm \
	-o ./third.bpf.ll third.bpf.c
llc -mcpu=probe -march=bpf -filetype=obj -o third.bpf.o third.bpf.ll
bpftool gen skeleton ./third.bpf.o name third > third.skel.h
