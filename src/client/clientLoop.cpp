
#include <pthread.h>
#include <string.h>

#include "clientLoop.hpp"
#include "protoDisconnectFromServer.hpp"
#include "protoRequestUserListFromServer.hpp"
#include "protoSendGlobalMessage.hpp"
#include "clientGetLineFromUser.hpp"
#include "clientPrintCommands.hpp"
#include "user.hpp"
#include "debug.hpp"

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
            DBG_ERROR("Unable to handle input.\n");
        }
        // disconect from the server
        if (strcmp(buffer, "/quit") == 0)
        {
            DBG_INFO("Goodbye!\n");
            g_is_alive = 0;
            status =
                protoDisconnectFromServer(gUser, "user initiated disconnect");
            if (status != OK)
            {
                DBG_ERROR("Unable to disconnect from server: %s\n",
                          PROTO_ERR_T_STRING[status]);
            }
            pthread_cancel(g_receive_thread);
            DBG_INFO("Joining receive thread...\n");
            pthread_join(g_receive_thread, NULL);
            DBG_INFO("Joined receive thread, terminating cmd loop\n");
            break;
        }
        // get a list of users connected to the server
        if (strcmp(buffer, "/getusers") == 0)
        {
            DBG_INFO("Requesting user list from server\n");
            status = protoRequestUserListFromServer(gUser);
            if (status != OK)
            {
                DBG_ERROR("Unable to get userlist from server: %s\n",
                          PROTO_ERR_T_STRING[status]);
            }
        }
        else
        {
            status = protoSendGlobalMessage(gUser, buffer);
            if (status != OK)
            {
                DBG_ERROR("Unable to get send message to server: %s\n",
                          PROTO_ERR_T_STRING[status]);
            }
        }
    }
    return OK;
}