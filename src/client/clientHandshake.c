
#include <string.h>
#include <unistd.h>  /* read */
#include <pthread.h> /* pthread */

#include "clientHandshake.h"
#include "clientReceiveFunction.h"
#include "protoSendClientName.h"
#include "errorCodes.h" /* proto_err_t */
#include "types.h"      /* command_t */
#include "protoSendClientName.h"
#include "wrappers.h"
#include "user.h"
#include "debug.h"

extern user_t *  gUser;
extern pthread_t g_receive_thread;   // thread to handle receiving data

proto_err_t clientHandshake(char *username)
{
    // the server will tell us something...
    command_t cmd;
    wrappers_memset(&cmd, 0, sizeof(command_t));
    wrappers_read(gUser, &cmd, sizeof(command_t));
    if (cmd.command_type == CMD_SERVER_REQUEST_NAME)
    {
        DBG_INFO("Sending client name to server...\n");
        protoSendClientName(gUser, username, strlen(username));
    }
    else
    {
        DBG_ERROR("Server asked for something other than a name\n");
        return ERR_INVALID_COMMAND;
    }

    // kick off receive thread to handle receipt of data
    pthread_create(&g_receive_thread, NULL, clientReceiveFunction, gUser);
    return OK;
}