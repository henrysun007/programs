#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int read_close_poll(void)
{
    int p[2];
    struct pollfd ufd;

    if (pipe(p) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    if (close(p[0]) < 0) { /* close the write fd */
        perror("close");
        return EXIT_FAILURE;
    }

    memset(&ufd, 0, sizeof ufd);
    ufd.fd = p[1]; /* poll the read fd after the write fd is closed */
    ufd.events = POLLIN|POLLPRI|POLLOUT|POLLRDHUP;
    if (poll(&ufd, 1, 1000) < 0) {
        perror("poll");
        return EXIT_FAILURE;
    }

    printf("read close revents: %#x\n", (unsigned)ufd.revents);

    return EXIT_SUCCESS;
}


int write_close_poll(void)
{
    int p[2];
    struct pollfd ufd;

    if (pipe(p) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    if (close(p[1]) < 0) { /* close the write fd */
        perror("close");
        return EXIT_FAILURE;
    }

    memset(&ufd, 0, sizeof ufd);
    ufd.fd = p[0]; /* poll the read fd after the write fd is closed */
    ufd.events = POLLIN|POLLPRI|POLLOUT|POLLRDHUP;
    if (poll(&ufd, 1, 1000) < 0) {
        perror("poll");
        return EXIT_FAILURE;
    }

    printf("write close revents: %#x\n", (unsigned)ufd.revents);

    return EXIT_SUCCESS;
}

int main() {
    read_close_poll();
    write_close_poll();
}
