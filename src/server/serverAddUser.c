
#include "serverAddUser.h"
#include "user.h"
#include "protocol.h"
#include "server.h"

extern server_state_t g_server_state;

/**
 * @brief register a user with the server
 *
 * @param user the user to register
 */
void serverAddUser(user_t *user)
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] == -1)
        {
            g_server_state.client_sockets[i] = user->client_socket_fd;
            break;
        }
    }
    g_server_state.connected_clients++;
    list_add(g_server_state.active_user_list, user);
}