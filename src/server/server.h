#ifndef __SERVER_H_
#define __SERVER_H_

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define DBG_INFO(fmt, ...)                                                   \
    do                                                                       \
    {                                                                        \
        fprintf(stderr, "%s:%d:%s INFO: " fmt, __FILE__, __LINE__, __func__, \
                ##__VA_ARGS__);                                              \
    } while (0)
#define DBG_ERROR(fmt, ...)                                                   \
    do                                                                        \
    {                                                                         \
        fprintf(stderr, "%s:%d:%s ERROR: " fmt, __FILE__, __LINE__, __func__, \
                ##__VA_ARGS__);                                               \
    } while (0)

#endif   // __SERVER_H_
