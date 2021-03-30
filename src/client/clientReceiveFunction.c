
#include <stdio.h>

#include "clientReceiveFunction.h"
#include "clientPrintPrompt.h"
#include "protoReadCommand.h"
#include "protocol.h"
#include "error_codes.h"
#include "wrappers.h"
#include "user.h"
#include "debug.h"

extern volatile int g_is_alive;

void *clientReceiveFunction(void *context)
{
    user_t *             user              = (user_t *)context;
    proto_err_t          status            = OK;
    command_t *          cmd               = NULL;
    name_list_t *        name_list         = NULL;
    broadcast_message_t *broadcast_message = NULL;

    DBG_INFO("Receive thread started...\n");
    while (g_is_alive)
    {
        // free any previously allocated command
        if (cmd)
        {
            wrappers_free(cmd);
            cmd = NULL;
        }

        clientPrintPrompt();
        status = protoReadCommand(user, &cmd);
        if (status != OK)
        {
            DBG_ERROR("Failed reading a command: %s\n",
                      PROTO_ERR_T_STRING[status]);
            if (status == ERR_REMOTE_HOST_CLOSED)
            {
                DBG_ERROR("Cannot recover from this error. Closing down...\n");
                break;
            }
            continue;
        }
        switch (cmd->command_type)
        {
            case CMD_SHARED_REQUEST_DISCONNECT:
                DBG_INFO(
                    "Server requested that we disconnect for reason %s. "
                    "Disconnecting...\n",
                    cmd->payload);
                break;
            case CMD_SERVER_USER_LIST:
                name_list = (name_list_t *)cmd->payload;
                DBG_INFO("%d users connected:\n", name_list->num_names);
                for (int i = 0; i < name_list->num_names; i++)
                {
                    DBG_INFO("Name: [%s]\n", name_list->usernames[i]);
                }
                break;
            case CMD_SERVER_BROADCAST_MESSAGE:
                broadcast_message = (broadcast_message_t *)cmd->payload;
                DBG_INFO("PUBLIC MESSAGE FROM [%s]: %s\n",
                         broadcast_message->name,
                         broadcast_message->message);
                break;
            default:
                DBG_ERROR("unknown command %d\n", cmd->command_type);
                break;
        }
    }   // end while alive loop

    DBG_INFO("Receive thread terminating.\n");
    if (cmd)
    {
        wrappers_free(cmd);
        cmd = NULL;
    }
    return NULL;
}