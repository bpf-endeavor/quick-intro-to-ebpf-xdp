#!/bin/bash


(sudo ./loader/loader &)
sleep 1
sudo tail -f /sys/kernel/tracing/trace_pipe
