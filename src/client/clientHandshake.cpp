
#include <string.h>
#include <unistd.h>  /* read */
#include <pthread.h> /* pthread */

#include "clientHandshake.hpp"
#include "clientReceiveFunction.hpp"
#include "protoSendClientName.hpp"
#include "errorCodes.hpp" /* proto_err_t */
#include "types.hpp"      /* command_t */
#include "protoSendClientName.hpp"
#include "wrappers.hpp"
#include "user.hpp"
#include "debug.hpp"

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