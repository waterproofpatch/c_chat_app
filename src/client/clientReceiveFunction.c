
#include <stdio.h>

#include "clientReceiveFunction.h"
#include "clientPrintPrompt.h"
#include "protoReadCommand.h"
#include "protocol.h"
#include "error_codes.h"
#include "wrappers.h"

extern volatile int g_is_alive;

void *clientReceiveFunction(void *context)
{
    int         sock_fd = *(int *)context;
    proto_err_t status  = OK;
    command_t * cmd     = NULL;
    printf("Receive thread started...\n");
    while (g_is_alive)
    {
        // free any previously allocated command
        if (cmd)
        {
            wrappers_free(cmd);
            cmd = NULL;
        }
        clientPrintPrompt();
        status = protoReadCommand(sock_fd, &cmd);
        if (status != OK)
        {
            printf("Failed reading a command: %s\n",
                   PROTO_ERR_T_STRING[status]);
            if (status == ERR_REMOTE_HOST_CLOSED)
            {
                printf("Cannot recover from this error. Closing down...\n");
                break;
            }
            continue;
        }
        name_list_t *        name_list         = NULL;
        broadcast_message_t *broadcast_message = NULL;
        switch (cmd->command_type)
        {
            case CMD_SHARED_REQUEST_DISCONNECT:
                printf(
                    "Server requested that we disconnect for reason %s. "
                    "Disconnecting...\n",
                    cmd->payload);
                break;
            case CMD_SERVER_USER_LIST:
                name_list = (name_list_t *)cmd->payload;
                printf("%d users connected:\n", name_list->num_names);
                for (int i = 0; i < name_list->num_names; i++)
                {
                    printf("Name: [%s]\n", name_list->usernames[i]);
                }
                break;
            case CMD_SERVER_BROADCAST_MESSAGE:
                broadcast_message = (broadcast_message_t *)cmd->payload;
                printf("%s: %s\n",
                       broadcast_message->name,
                       broadcast_message->message);
                break;
            default:
                printf("unknown command %d\n", cmd->command_type);
                break;
        }
    }   // end while alive loop
    printf("Receive thread terminating.\n");
    if (cmd)
    {
        wrappers_free(cmd);
        cmd = NULL;
    }
    return NULL;
}