CC = gcc
CFLAGS = -O2 -Wall -pthread

all: memkv_rdma_sync

memkv_rdma_sync: src/memkv_rdma_sync.c
	$(CC) $(CFLAGS) $< -o $@ -libverbs

clean:
	rm -f memkv_rdma_sync
