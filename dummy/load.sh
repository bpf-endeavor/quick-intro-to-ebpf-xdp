#! /bin/bash
ip link set dev veth2 xdp off
sudo ip link set dev veth2 xdp obj first.bpf.o sec xdp 

on_signal() {
	ip link set dev veth2 xdp off
	exit 0
}

trap "on_signal" SIGINT SIGHUP
echo Hit Ctrl-C
while [[ true ]]; do
	sleep 5
done
