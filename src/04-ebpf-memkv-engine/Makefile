CC = gcc
CLANG = clang
CFLAGS = -O2 -Wall
BPF_CFLAGS = -O2 -g -Wall -target bpf

all: memkv_kernel.o memkv_cli

memkv_kernel.o: src/memkv_kernel.c
	$(CLANG) $(BPF_CFLAGS) -c $< -o $@

memkv_cli: src/memkv_cli.c
	$(CC) $(CFLAGS) $< -o $@ -lbpf -lelf

clean:
	rm -f memkv_kernel.o memkv_cli
