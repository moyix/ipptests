#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

// Note: SLOTS not used here because we don't have control
// over the size of the kernel pipe buffer.

int iters;

void produce(int fd) {
    int dev = open(DEV, O_RDONLY);
    char buf[CHUNK];
    for (int i = 0; i < iters; i++) {
        read(dev, buf, CHUNK);
        write(fd, buf, CHUNK);
    }
    close(fd);
    close(dev);
}

void consume(int fd) {
    char rbuf[CHUNK];
    int n = 0;
    while (0 != read(fd, rbuf, CHUNK)) n++;
    assert (n == iters);
    close(fd);
}

int main(int argc, char **argv) {
    int fds[2];
    if (argc < 2) {
        fprintf(stderr, "usage: %s <iters>\n", argv[0]);
        return 1;
    }

    iters = atoi(argv[1]);

    if (fork() == 0) {
        pipe(fds);
        if (fork() == 0) {
            close(fds[0]);
            produce(fds[1]);
        }
        else {
            close(fds[1]);
            consume(fds[0]);
        }
    }
    else {
        wait(NULL);
        wait(NULL);
    }

    return 0;
}
