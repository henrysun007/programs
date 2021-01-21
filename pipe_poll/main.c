#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void print_r(unsigned short r) {
    printf("0x%04x ", (unsigned)r);
    if ((r & POLLIN) != 0) printf("POLLIN | ");
    if ((r & POLLOUT) != 0) printf("POLLOUT | ");
    if ((r & POLLPRI) != 0) printf("POLLPRI | ");
    if ((r & POLLERR) != 0) printf("POLLERR | ");
    if ((r & POLLHUP) != 0) printf("POLLHUP | ");
    if ((r & POLLRDNORM) != 0) printf("POLLRDNORM| ");
    if ((r & POLLRDBAND) != 0) printf("POLLRDBAND | ");
    if ((r & POLLWRNORM) != 0) printf("POLLWRNORM | ");
    if ((r & POLLWRBAND) != 0) printf("POLLWRBAND | ");
    if ((r & POLLMSG) != 0) printf("POLLMSG | ");
    if ((r & POLLRDHUP) != 0) printf("POLLRDHUP | ");
}

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

    printf("reader close writer revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
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

    printf("writer close reader revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
    return EXIT_SUCCESS;
}

int no_close_poll(void)
{
    int p[2];
    struct pollfd ufd;

    if (pipe(p) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    memset(&ufd, 0, sizeof ufd);
    ufd.fd = p[0];
    ufd.events = POLLIN|POLLPRI|POLLOUT|POLLRDHUP;
    if (poll(&ufd, 1, 1000) < 0) {
        perror("poll");
        return EXIT_FAILURE;
    }

    printf("reader revents: ");
    print_r((unsigned)ufd.revents);

    memset(&ufd, 0, sizeof ufd);
    ufd.fd = p[1];
    ufd.events = POLLIN|POLLPRI|POLLOUT|POLLRDHUP;
    if (poll(&ufd, 1, 1000) < 0) {
        perror("poll");
        return EXIT_FAILURE;
    }

    printf("writer revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
    return EXIT_SUCCESS;
}

int main() {
    no_close_poll();
    read_close_poll();
    write_close_poll();
}
