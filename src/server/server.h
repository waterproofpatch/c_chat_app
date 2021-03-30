#ifndef __SERVER_H_
#define __SERVER_H_

#include <sys/select.h>
#include <stdio.h>
#include <openssl/ssl.h>

#include "types.h"
#include "constants.h"
#include "errorCodes.h"

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

#endif
