clang -S \
	-target bpf \
	-g -O2 -emit-llvm \
	-o ./first.bpf.ll first.bpf.c
llc -mcpu=probe -march=bpf -filetype=obj -o first.bpf.o first.bpf.ll
bpftool gen skeleton ./first.bpf.o name first > first.skel.h
