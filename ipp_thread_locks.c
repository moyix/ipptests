#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

int iters;
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
int count = 0;
char bufdata[CHUNK*SLOTS];

void * produce(void *arg) {
    int dev = open(DEV, O_RDONLY);
    char buf[CHUNK];
    for (int i = 0; i < iters; i++) {
        read(dev, buf, CHUNK);
        pthread_mutex_lock(&the_mutex);
        while (count == SLOTS)
            pthread_cond_wait(&condp, &the_mutex);
        memcpy(bufdata+(count*CHUNK), buf, CHUNK);
        count += 1;
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&the_mutex);
    }
    close(dev);
    pthread_exit(0);
}

void * consume(void *arg) {
    char rbuf[CHUNK];
    for (int i = 0; i < iters; i++) {
        pthread_mutex_lock(&the_mutex);
        while (count == 0)
            pthread_cond_wait(&condc, &the_mutex);
        memcpy(rbuf, bufdata+(count*CHUNK), CHUNK);
        count -= 1;
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&the_mutex);
    }
    pthread_exit(0);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <iters>\n", argv[0]);
        return 1;
    }

    iters = atoi(argv[1]);

    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condc, NULL);
    pthread_cond_init(&condp, NULL);

    pthread_t producer, consumer;
    pthread_create(&producer, NULL, produce, NULL);
    pthread_create(&consumer, NULL, consume, NULL);

    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);

    return 0;
}
