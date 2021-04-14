
#include <string.h>

#include "protoSendGlobalMessage.hpp"
#include "protoSendCommand.hpp"
#include "types.hpp"
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoSendGlobalMessage(user_t *user, char *buffer)
{
    return protoSendCommand(
        user, CMD_CLIENT_BROADCAST_MESSAGE, buffer, strlen(buffer));
}