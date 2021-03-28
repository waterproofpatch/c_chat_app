
#include <string.h> /* strlen */
#include "protoDisconnectClient.h"
#include "protoSendCommand.h"
#include "error_codes.h" /* proto_err_t */
#include "protocol.h"    /* CMD */
#include "user.h"

proto_err_t protoDisconnectClient(user_t *user, const char *reason)
{
    proto_err_t status = OK;

    status = protoSendCommand(
        user, CMD_SHARED_REQUEST_DISCONNECT, reason, strlen(reason));
    return status;
}