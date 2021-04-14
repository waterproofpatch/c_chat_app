
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define DBG_INFO(fmt, ...)             \
    do                                 \
    {                                  \
        fprintf(stderr,                \
                "%s:%d:%s INFO: " fmt, \
                __FILE__,              \
                __LINE__,              \
                __func__,              \
                ##__VA_ARGS__);        \
    } while (0)
#define DBG_ERROR(fmt, ...)             \
    do                                  \
    {                                   \
        fprintf(stderr,                 \
                "%s:%d:%s ERROR: " fmt, \
                __FILE__,               \
                __LINE__,               \
                __func__,               \
                ##__VA_ARGS__);         \
    } while (0)

#endif