
#include <pthread.h>
#include <string.h>

#include "clientLoop.h"
#include "protoDisconnectFromServer.h"
#include "protoRequestUserListFromServer.h"
#include "protoSendGlobalMessage.h"
#include "clientGetLineFromUser.h"
#include "clientPrintCommands.h"
#include "user.h"

extern volatile int g_is_alive;
extern user_t *     gUser;
extern pthread_t    g_receive_thread;   // thread to handle receiving data

proto_err_t clientLoop()
{
    clientPrintCommands();
    while (1)
    {
        char        buffer[256];
        proto_err_t status = OK;
        if (clientGetLineFromUser(buffer, sizeof(buffer)) != 1)
        {
            printf("Unable to handle input.\n");
        }
        // disconect from the server
        if (strcmp(buffer, "/quit") == 0)
        {
            printf("Goodbye!\n");
            g_is_alive = 0;
            status =
                protoDisconnectFromServer(gUser, "user initiated disconnect");
            if (status != OK)
            {
                printf("Unable to disconnect from server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
            pthread_cancel(g_receive_thread);
            printf("Joining receive thread...\n");
            pthread_join(g_receive_thread, NULL);
            printf("Joined receive thread, terminating cmd loop\n");
            break;
        }
        // get a list of users connected to the server
        if (strcmp(buffer, "/getusers") == 0)
        {
            printf("Requesting user list from server\n");
            status = protoRequestUserListFromServer(gUser);
            if (status != OK)
            {
                printf("Unable to get userlist from server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
        }
        else
        {
            status = protoSendGlobalMessage(gUser, buffer);
            if (status != OK)
            {
                printf("Unable to get send message to server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
        }
    }
    return OK;
}