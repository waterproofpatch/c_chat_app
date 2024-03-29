
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

#include "wrappers.hpp"
#include "user.hpp"

void *wrappers_malloc(size_t size)
{
    return malloc(size);
}

void wrappers_free(void *p)
{
    return free(p);
}

int wrappers_bind(int                    socket,
                  const struct sockaddr *address,
                  socklen_t              address_len)
{
    return bind(socket, address, address_len);
}

int wrappers_connect(int                    socket,
                     const struct sockaddr *address,
                     socklen_t              address_len)
{
    return connect(socket, address, address_len);
}

int wrappers_listen(int socket, int backlog)
{
    return listen(socket, backlog);
}

int wrappers_setsockopt(int         socket,
                        int         level,
                        int         option_name,
                        const void *option_value,
                        socklen_t   option_len)
{
    return setsockopt(socket, level, option_name, option_value, option_len);
}
int wrappers_getsockopt(int        socket,
                        int        level,
                        int        option_name,
                        void *     option_value,
                        socklen_t *option_len)
{
    return getsockopt(socket, level, option_name, option_value, option_len);
}

void *wrappers_memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}
void *wrappers_memcpy(void *dst, void *src, size_t n)
{
    return memcpy(dst, src, n);
}

ssize_t wrappers_write(user_t *user, void *buf, size_t nbyte)
{
    return SSL_write(user->ssl, buf, nbyte);
}

ssize_t wrappers_read(user_t *user, void *buf, size_t nbyte)
{
    return SSL_read(user->ssl, buf, nbyte);
}

int wrappers_close(int fd)
{
    return close(fd);
}

int wrappers_socket(int domain, int type, int protocol)
{
    return socket(domain, type, protocol);
}

int wrappers_select(int             ndfs,
                    fd_set *        readfds,
                    fd_set *        writefds,
                    fd_set *        errorfds,
                    struct timeval *timeout)
{
    return select(ndfs, readfds, writefds, errorfds, timeout);
}
