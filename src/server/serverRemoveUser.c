
#include "serverRemoveUser.h"
#include "user.h"
#include "wrappers.h"
#include "list.h"
#include "server.h"
#include "constants.h"

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
        if (g_server_state.client_sockets[i] == user)
        {
            wrappers_close(user->client_socket_fd);
            list_remove(g_server_state.active_user_list, user);
            g_server_state.client_sockets[i]->client_socket_fd = -1;

            wrappers_free(user);
            g_server_state.client_sockets[i] = NULL;
            g_server_state.connected_clients--;

            break;
        }
    }
}