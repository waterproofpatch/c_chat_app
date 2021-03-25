#include "serverGetClientSocket.h"
#include "server.h"

extern server_state_t g_server_state;

/**
 * @brief get the file descriptor for client at index
 *
 * @param index into the global file descriptors table
 * @return int the fd
 * @return  -1 if not found or not init
 */
int serverGetClientSocket(int index)
{
    if (index >= MAX_CLIENTS)
    {
        return -1;
    }
    return g_server_state.client_sockets[index];
}