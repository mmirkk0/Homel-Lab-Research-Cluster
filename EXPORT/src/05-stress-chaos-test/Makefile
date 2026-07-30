CC = gcc
CFLAGS = -O2 -Wall -pthread

all: rdma_saturator cache_thrash_saturator

rdma_saturator: src/rdma_saturator.c
	$(CC) $(CFLAGS) $< -o $@ -libverbs

cache_thrash_saturator: src/cache_thrash_saturator.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f rdma_saturator cache_thrash_saturator
