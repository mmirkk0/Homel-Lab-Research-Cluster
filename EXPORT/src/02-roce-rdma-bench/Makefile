CC = gcc
CFLAGS = -O2 -Wall

all: roce_bench

roce_bench: src/roce_bench.c
	$(CC) $(CFLAGS) $< -o $@ -libverbs

clean:
	rm -f roce_bench
