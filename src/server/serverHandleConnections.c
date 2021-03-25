
#include <string.h>
#include <errno.h>

#include "server.h"
#include "serverHandleConnections.h"
#include "serverUpdateMaxFd.h"
#include "serverGetClientSocket.h"
#include "serverProcessNewClient.h"
#include "serverRemoveUser.h"
#include "serverClientSocketFdComparator.h"
#include "protoBroadcastMessage.h"
#include "protoReadCommand.h"
#include "protoPrintCommand.h"
#include "protoSendUserList.h"
#include "wrappers.h"
#include "list.h"

extern server_state_t g_server_state;

void serverHandleConnections(void)
{
    serverUpdateMaxFd();

    int num_ready_fd = wrappers_select(
        g_server_state.max_fd + 1, &g_server_state.all_fds, NULL, NULL, NULL);
    if (num_ready_fd < 0 && errno != EINTR)
    {
        DBG_INFO("Select error %d\n", errno);
        return;
    }

    // if the activity was on our listening socket, it's a new connection
    if (FD_ISSET(g_server_state.server_sock_fd, &g_server_state.all_fds))
    {
        if (g_server_state.connected_clients == MAX_CLIENTS)
        {
            DBG_INFO("Cannot accept any new clients!\n");
            return;
        }
        else
        {
            DBG_INFO("Processing new connection...\n");
            if (serverProcessNewClient() == OK)
            {
                DBG_INFO("Accepted new client!\n");
            }
            else
            {
                DBG_INFO("Error processing new connection...\n");
            }
            return;
        }
    }

    // otherwise we have a message from an already connected client
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        int sd = serverGetClientSocket(i);
        // if we don't have any stored fds or if the stored fd isn't the one
        // who woke us up, continue
        if (sd == -1 || !FD_ISSET(sd, &g_server_state.all_fds))
        {
            continue;
        }

        user_t *user = list_search(g_server_state.active_user_list,
                                   serverClientSocketFdComparator,
                                   &sd);
        if (!user)
        {
            DBG_INFO(
                "Someone is talking on socket %d, but I don't know "
                "who...\n",
                sd);
            continue;   // this is a problem
        }
        DBG_INFO("User [%s] has a message for the server.\n", user->name);

        command_t * cmd    = NULL;
        proto_err_t status = OK;
        status             = protoReadCommand(sd, &cmd);
        if (status != OK || cmd->command_type == CMD_SHARED_REQUEST_DISCONNECT)
        {
            DBG_INFO("Status is %s, disconnecting client %s\n",
                     PROTO_ERR_T_STRING[status],
                     user->name);
            serverRemoveUser(user);
        }
        else if (cmd->command_type == CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER)
        {
            DBG_INFO("User [%s] requests user list from server...\n",
                     user->name);
            protoSendUserList(sd, g_server_state.active_user_list);
        }
        else if (cmd->command_type == CMD_CLIENT_BROADCAST_MESSAGE)
        {
            DBG_INFO("User %s sent a message to everyone: %s\n",
                     user->name,
                     cmd->payload);
            // send a message to *each* client
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (serverGetClientSocket(j) >= 0)
                {
                    protoBroadcastMessage(serverGetClientSocket(j),
                                            user->name,
                                            strlen(user->name),
                                            cmd->payload,
                                            strlen(cmd->payload));
                }
            }
        }
        else
        {
            DBG_INFO("User [%s] send a command:\n", user->name);
            protoPrintCommand(cmd);
        }
    }
    return;
}