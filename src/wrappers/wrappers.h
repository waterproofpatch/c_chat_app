#ifndef _WRAPPERS_H
#define _WRAPPERS_H
/**
 * @file wrappers.h 
 * @brief wrappers around stdlib to facilitate mocking
 * @version 0.1
 * @date 2018-10-07
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>

void* wrappers_malloc(size_t size);

void wrappers_free(void* p);

void* wrappers_memset(void* b, int c, size_t len);

void* wrappers_memcpy(void* dst, void* src, size_t n);

ssize_t wrappers_write(int filedes, void* buf, size_t nbyte);

ssize_t wrappers_read(int filedes, void* buf, size_t nbyte);

#endif