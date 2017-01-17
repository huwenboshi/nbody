CC= g++
COPTS= -Wall -O3

NVCC=nvcc
NVLIBS=-L/u/local/cuda/current/lib64 -lcuda -lcudart
NVINCLUDES=-I/u/local/cuda/current/include
NVOPTS=-O3 -arch sm_20 -use_fast_math

all: main

main: main.o utils.o sim.o utils_gpu.o sim_gpu.o
	$(CC) main.o utils.o sim.o utils_gpu.o sim_gpu.o -o main \
		$(COPTS) $(NVLIBS)

main.o: main.cpp
	$(CC) -c main.cpp $(COPTS)

utils.o: utils.cpp utils.h
	$(CC) -c utils.cpp $(COPTS)

utils_gpu.o: utils_gpu.cu utils.h
	$(NVCC) -c utils_gpu.cu $(NVINCLUDES) $(NVOPTS)

sim.o: sim.cpp sim.h
	$(CC) -c sim.cpp $(COPTS)

sim_gpu.o: sim_gpu.cu sim_gpu.h
	$(NVCC) -c sim_gpu.cu $(NVINCLUDES) $(NVOPTS)

clean:
	rm -f *.o main
