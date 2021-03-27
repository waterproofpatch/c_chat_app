
#include <string.h> /* strlen */
#include "protoDisconnectClient.h"
#include "protoSendCommand.h"
#include "error_codes.h" /* proto_err_t */
#include "protocol.h"    /* CMD */

proto_err_t protoDisconnectClient(int sock_fd, char *reason)
{
    proto_err_t status = OK;

    status = protoSendCommand(
        sock_fd, CMD_SHARED_REQUEST_DISCONNECT, reason, strlen(reason));
    return status;
}