# What is this?

Our experiment environment is built with Veth interfaces.
Due to their implementation details, the Veth interface should have a XDP
program attached if it wants to receive packets from other interfaces redirect
packets.

This is a dummy XDP program (similar to the first XDP program in the hands-on
session) that you can attach it inside the network namespace (to `veth2`) so
that you can receive packets redirected from XDP program attached to `veth1`
(useful for part3).

