#pragma once

#define MAX_STR_SIZE 32
struct key {
	char data[MAX_STR_SIZE];
} __attribute__((packed));

struct value {
	char data[MAX_STR_SIZE];
} __attribute__((packed));
