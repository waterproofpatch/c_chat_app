#include <string.h>

#include "protoDisconnectFromServer.hpp"
#include "protoSendCommand.hpp"
#include "errorCodes.hpp" /* proto_err_t */
#include "types.hpp"      /* CMD */
#include "user.hpp"

proto_err_t protoDisconnectFromServer(user_t *user, const char *reason)
{
    proto_err_t status = OK;
    status             = protoSendCommand(
        user, CMD_SHARED_REQUEST_DISCONNECT, reason, strlen(reason));
    return status;
}