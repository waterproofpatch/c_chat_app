
#include "serverRemoveUser.h"
#include "user.h"
#include "wrappers.h"
#include "list.h"
#include "server.h"

extern server_state_t g_server_state;

/**
 * @brief remove a client from the server
 *
 * @param user the user to remove
 */
void serverRemoveUser(user_t *user)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] == user->client_socket_fd)
        {
            g_server_state.client_sockets[i] = -1;
            break;
        }
    }
    wrappers_close(user->client_socket_fd);
    g_server_state.connected_clients--;
    list_remove(g_server_state.active_user_list, user);
    wrappers_free(user);
}