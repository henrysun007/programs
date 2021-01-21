#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int sd_poll(int how)
{
    int p[2];
    struct pollfd ufd;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    if (shutdown(p[0], how) < 0) { /* close the write fd */
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

    printf("how %d revents: %#x\n", how, (unsigned)ufd.revents);
    return EXIT_SUCCESS;
}

int no_sd_poll()
{
    int p[2];
    struct pollfd ufd;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }

    memset(&ufd, 0, sizeof ufd);
    ufd.fd = p[1]; /* poll the read fd after the write fd is closed */
    ufd.events = POLLIN|POLLPRI|POLLOUT|POLLRDHUP;
    if (poll(&ufd, 1, 1000) < 0) {
        perror("poll");
        return EXIT_FAILURE;
    }

    printf("revents: %#x\n", (unsigned)ufd.revents);
    return EXIT_SUCCESS;
}

int main() {
    no_sd_poll();
    sd_poll(SHUT_RD);
    sd_poll(SHUT_WR);
    sd_poll(SHUT_RDWR);

    return EXIT_SUCCESS;
}
