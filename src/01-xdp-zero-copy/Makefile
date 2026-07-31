CC = gcc
CLANG = clang
CFLAGS = -O2 -Wall
BPF_CFLAGS = -O2 -g -Wall -target bpf

all: xdp_prog.o xdp_loader

xdp_prog.o: src/xdp_prog.c
	$(CLANG) $(BPF_CFLAGS) -c $< -o $@

xdp_loader: src/xdp_loader.c
	$(CC) $(CFLAGS) $< -o $@ -lbpf -lelf

clean:
	rm -f xdp_prog.o xdp_loader
