/**
 * @file wrappers.h
 * @brief wrappers around stdlib to facilitate mocking
 * @version 0.1
 * @date 2018-10-07
 *
 * @copyright Copyright (c) 2018
 *
 */
#ifndef _WRAPPERS_H
#define _WRAPPERS_H

#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

void *wrappers_malloc(size_t size);

int wrappers_connect(int                    socket,
                     const struct sockaddr *address,
                     socklen_t              address_len);
int wrappers_bind(int                    socket,
                  const struct sockaddr *address,
                  socklen_t              address_len);

int wrappers_listen(int socket, int backlog);

int wrappers_setsockopt(int         socket,
                        int         level,
                        int         option_name,
                        const void *option_value,
                        socklen_t   option_len);

int wrappers_getsockopt(int        socket,
                        int        level,
                        int        option_name,
                        void *     option_value,
                        socklen_t *option_len);

int wrappers_close(int fd);

void wrappers_free(void *p);

void *wrappers_memset(void *b, int c, size_t len);

void *wrappers_memcpy(void *dst, void *src, size_t n);

ssize_t wrappers_write(SSL *ssl, void *buf, size_t nbyte);

ssize_t wrappers_read(SSL *ssl, void *buf, size_t nbyte);

int wrappers_socket(int domain, int type, int protocol);

int wrappers_select(int             ndfs,
                    fd_set *        readfds,
                    fd_set *        writefds,
                    fd_set *        errorfds,
                    struct timeval *timeout);

#endif   // _WRAPPERS_H