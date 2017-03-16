#CFLAGS = -O3
#CXXFLAGS = -O3

ipp_thread: CXXFLAGS += -pthread -std=c++11
ipp_thread_locks: CFLAGS += -pthread

all: ipp_thread ipp_thread_locks ipp_proc

bench:
	time taskset -c 0 ./ipp_proc 10000000
	time taskset -c 0 ./ipp_thread 10000000
	time taskset -c 0 ./ipp_thread_locks 10000000

clean:
	rm -f *.o ipp_thread ipp_thread_locks ipp_proc
