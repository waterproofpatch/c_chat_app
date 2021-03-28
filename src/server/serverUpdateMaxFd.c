#include <sys/select.h>

#include "serverUpdateMaxFd.h"
#include "server.h"

extern server_state_t g_server_state;

/**
 * @brief update the max FD value.
 *
 */
void serverUpdateMaxFd(void)
{
    FD_ZERO(&g_server_state.all_fds);
    FD_SET(g_server_state.server_sock_fd, &g_server_state.all_fds);
    g_server_state.max_fd = g_server_state.server_sock_fd;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] != NULL)
        {
            FD_SET(g_server_state.client_sockets[i]->client_socket_fd,
                   &g_server_state.all_fds);
            g_server_state.max_fd =
                MAX(g_server_state.max_fd,
                    g_server_state.client_sockets[i]->client_socket_fd);
        }
    }
}