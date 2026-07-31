CC = gcc
CFLAGS = -O2 -Wall -pthread

all: gpu_offload_sim

gpu_offload_sim: src/gpu_offload_sim.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f gpu_offload_sim
