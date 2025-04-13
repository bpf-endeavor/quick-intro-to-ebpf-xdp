First eBPF Program
==================

.. code:: c++

    #include <linux/bpf.h>
    #include <bpf/bpf_helpers.h>

    SEC("xdp")
    int xdp_main(struct xdp_md *ctx)
    {
        bpf_printk("here");
        return XDP_PASS;
    }

    char LICENSE[] SEC("license") = "GPL";


First Time Running eBPF Program
================================

**Setup Environment:**

.. code:: sh

    sudo ip netns add n2
    sudo ip link add veth1 type veth peer name veth2 netns n2
    sudo ip link set veth1 up
    sudo ip addr add 10.10.0.1/24 dev veth1

    sudo ip netns exec n2 ip link set veth2 up
    sudo ip netns exec n2 ip addr add 10.10.0.2/24 dev veth2


.. code:: sh

    +---------------------------------------------+
    |                                             |
    | 10.10.0.1           +----------------+      |
    |      +-----+        |-----+          |      |
    |      |veth1|<------>|veth2| 10.10.0.2|      |
    |      +-----+        |-----+          |      |
    |         I           | Attach Dummy   |      |
    |  Attach XDP         |                |      |
    |                     +----------------+      |
    |                                             |
    |                                             |
    |                                             |
    |                                             |
    +---------------------------------------------+


**Compile eBPF Program:**

.. code:: sh

    clang -S \
      -target bpf \
      -g -O2 -emit-llvm \
      -o NAME.bpf.ll NAME.bpf.c

    llc -mcpu=probe -march=bpf -filetype=obj -o NAME.bpf.o NAME.bpf.ll

    bpftool gen skeleton NAME.bpf.o name SKEL_NAME > NAME.skel.h


**Compile Loader Program:**

.. code:: sh

    clang -g -O2 -o ./loader ./loader.c -lbpf


**Reading BPF Trace Logs:**

.. code:: sh

    sudo cat /sys/kernel/tracing/trace_pipe


BPFTOOL
=======

**Listing attached eBPF Networking Programs:**

.. code:: sh

    sudo bpftool net

**Listing Loaded eBPF Programs**

.. code:: sh

    sudo bpftool prog

Generating Packets
==================

**Running NetCat Server (listen for packets):**

.. code:: sh

    nc -l -u 10.10.0.1 8080

**Running NetCat Sending Packets:**

.. code:: sh

    printf "hello world\n" | nc -W 1 -N -u 10.10.0.1 8080


Using IPROUTE2 To Load XDP Programs
===================================

.. code:: sh

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

