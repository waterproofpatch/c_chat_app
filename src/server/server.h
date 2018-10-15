#ifndef __SERVER_H_
#define __SERVER_H_

#include "protocol.h"

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

/**
 * @brief call in a loop to handle connections once the server is created.
 * 
 */
void server_handle_connections();

/**
 * @brief call at the beginning to set up the client sockets and bind the server to the specified port
 * 
 * @param port_no the port to bind
 * @return proto_err_t  OK if the server was started successfully, ERR_* otherwise
 */
proto_err_t server_create(unsigned short port_no);

#endif   // __SERVER_H_
