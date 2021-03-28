
#include <string.h>

#include "protoSendGlobalMessage.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "error_codes.h"
#include "user.h"

proto_err_t protoSendGlobalMessage(user_t *user, char *buffer)
{
    return protoSendCommand(
        user, CMD_CLIENT_BROADCAST_MESSAGE, buffer, strlen(buffer));
}