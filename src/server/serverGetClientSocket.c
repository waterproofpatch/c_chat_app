#include "serverGetClientSocket.h"
#include "server.h"
#include "user.h"

extern server_state_t g_server_state;

/**
 * @brief get the file descriptor for client at index.
 *
 * @param index into the global file descriptors table.
 * @return ptr to the user at index.
 * @return  NULL if not found or not init.
 */
user_t *serverGetClientSocket(int index)
{
    if (index >= MAX_CLIENTS)
    {
        return NULL;
    }
    return g_server_state.client_sockets[index];
}