
#include <stdio.h> /* printf */
#include <string.h>
#include <unistd.h>  /* read */
#include <pthread.h> /* pthread */

#include "clientHandshake.h"
#include "clientReceiveFunction.h"
#include "protoSendClientName.h"
#include "error_codes.h" /* proto_err_t */
#include "protocol.h"    /* command_t */
#include "protoSendClientName.h"

extern int       g_sock_fd;   // socket descriptor for the client connection
extern pthread_t g_receive_thread;   // thread to handle receiving data

proto_err_t clientHandshake(char *username)
{
    // the server will tell us something...
    command_t cmd = {0};
    read(g_sock_fd, &cmd, sizeof(command_t));
    if (cmd.command_type == CMD_SERVER_REQUEST_NAME)
    {
        printf("Sending client name to server...\n");
        protoSendClientName(g_sock_fd, username, strlen(username));
    }
    else
    {
        printf("Server asked for something other than a name\n");
        return ERR_INVALID_COMMAND;
    }

    // kick off receive thread to handle receipt of data
    pthread_create(&g_receive_thread, NULL, clientReceiveFunction, &g_sock_fd);
    return OK;
}