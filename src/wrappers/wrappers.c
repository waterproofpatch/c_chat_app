
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include "wrappers.h"

void* wrappers_malloc(size_t size)
{
    return malloc(size);
}

void wrappers_free(void* p)
{
    return free(p);
}

void* wrappers_memset(void* b, int c, size_t len)
{
    return memset(b, c, len);
}
void* wrappers_memcpy(void* dst, void* src, size_t n)
{
    return memcpy(dst, src, n);
}

ssize_t wrappers_write(int filedes, void* buf, size_t nbyte)
{
    return write(filedes, buf, nbyte);
}

ssize_t wrappers_read(int filedes, void* buf, size_t nbyte)
{
    return read(filedes, buf, nbyte);
}

int wrappers_close(int fd)
{
    return close(fd);
}

int wrappers_socket(int domain, int type, int protocol)
{
    return socket(domain, type, protocol);
}
int wrappers_select(int     ndfs,
                    fd_set* restrict readfds,
                    fd_set* restrict writefds,
                    fd_set* restrict errorfds,
                    struct timeval* restrict timeout)
{
    return select(ndfs, readfds, writefds, errorfds, timeout);
}