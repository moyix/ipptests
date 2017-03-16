#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <array>

#include "common.h"

#include "circularfifo_memory_sequential_consistent.hpp"
using namespace memory_sequential_consistent;

int iters;
CircularFifo <std::array<char,CHUNK>, SLOTS> queue;

void * produce(void *arg) {
    int dev = open(DEV, O_RDONLY);
    std::array<char, CHUNK> buf;
    for (int i = 0; i < iters; i++) {
        read(dev, buf.data(), buf.size());
        while (false == queue.push(buf)) pthread_yield();
    }
    close(dev);
    pthread_exit(0);
}

void * consume(void *arg) {
    std::array<char, CHUNK> rbuf;
    for (int i = 0; i < iters; i++) {
        while (false == queue.pop(rbuf)) pthread_yield();
    }
    pthread_exit(0);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <iters>\n", argv[0]);
        return 1;
    }

    iters = atoi(argv[1]);

    pthread_t producer, consumer;
    pthread_create(&producer, NULL, produce, NULL);
    pthread_create(&consumer, NULL, consume, NULL);

    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);

    return 0;
}
