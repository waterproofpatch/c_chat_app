#ifndef __SERVER_H_
#define __SERVER_H_

#include <sys/select.h>
#include <stdio.h>

#include <openssl/ssl.h>

#include "protocol.h"

#include "debug.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * @brief encapsulate server global state
 *
 */
typedef struct _server_state_t
{
    user_t *client_sockets[MAX_CLIENTS];   // each client's fd
    int     connected_clients;   // number of currently connected clients
    int     max_fd;              // max file descriptor
    int     server_sock_fd;      // server socket
    list_t *active_user_list;    // list of connected users
    fd_set  all_fds;             // set file descriptors
} server_state_t;

/**
 * @brief call in a loop to handle connections once the server is created.
 *
 */
void server_handle_connections();

/**
 * @brief call at the beginning to set up the client sockets and bind the server
 * to the specified port
 *
 * @param port_no the port to bind
 * @return proto_err_t OK if the server was started successfully, ERR_*
 * otherwise
 */
proto_err_t server_create(unsigned short port_no);

#endif   // __SERVER_H_
