#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

void print_how(int how) {

    switch(how) {
        case 0: 
            printf("SHUT_RD   ");
            break;
        case 1: 
            printf("SHUT_WR   ");
            break;
        case 2: 
            printf("SHUT_RDWR ");
            break;
        default:
            printf("wrong number");
    }
}

void print_r(unsigned short r) {

    printf("0x%04x ", (unsigned)r);
    if ((r & POLLIN) != 0) printf("POLLIN | ");
    if ((r & POLLOUT) != 0) printf("POLLOUT | ");
    if ((r & POLLPRI) != 0) printf("POLLPRI | ");
    if ((r & POLLERR) != 0) printf("POLLERR | ");
    if ((r & POLLIN) != 0) printf("POLLIN | ");
    if ((r & POLLHUP) != 0) printf("POLLHUP | ");
    if ((r & POLLRDNORM) != 0) printf("POLLRDNORM| ");
    if ((r & POLLRDBAND) != 0) printf("POLLRDBAND | ");
    if ((r & POLLWRNORM) != 0) printf("POLLWRNORM | ");
    if ((r & POLLWRBAND) != 0) printf("POLLWRBAND | ");
    if ((r & POLLMSG) != 0) printf("POLLMSG | ");
    if ((r & POLLRDHUP) != 0) printf("POLLRDHUP | ");
    
}

int sd_poll(int c_how, int r_how)
{
    int p[2];
    struct pollfd ufd;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    
    if (shutdown(p[0], r_how) < 0) { 
        perror("close");
        return EXIT_FAILURE;
    }
    

    if (shutdown(p[1], c_how) < 0) { /* close the write fd */
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

    printf("current_how: ");
    print_how(c_how);
    printf("remote_how: "); 
    print_how(r_how);
    printf("revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
    return EXIT_SUCCESS;
}

int r_sd_poll(int r_how)
{
    int p[2];
    struct pollfd ufd;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    
    if (shutdown(p[0], r_how) < 0) { 
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

    printf("                       ");
    printf("remote_how: "); 
    print_how(r_how);
    printf("revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
    return EXIT_SUCCESS;
}

int c_sd_poll(int c_how)
{
    int p[2];
    struct pollfd ufd;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    
    if (shutdown(p[1], c_how) < 0) { /* close the write fd */
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

    printf("current_how: ");
    print_how(c_how);
    printf("                      ");
    printf("revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
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

    printf("                      ");
    printf("                       ");
    printf("revents: ");
    print_r((unsigned)ufd.revents);
    printf("\n");
    return EXIT_SUCCESS;
}

int main() {
    int i, j;

    no_sd_poll();

    for (i = 0; i < 3; i++) {
        c_sd_poll(i);
    }

    for (i = 0; i < 3; i++) {
        r_sd_poll(i);
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++)
            sd_poll(i, j);
    }

    return EXIT_SUCCESS;
}
