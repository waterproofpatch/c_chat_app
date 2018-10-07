#ifndef _WRAPPERS_H
#define _WRAPPERS_H

#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Wrapper for malloc
 *
 * @param size number of bytes to allocate
 * @return void* ptr to allocated data, or NULL
 */
void* wrappers_malloc(size_t size);

#endif