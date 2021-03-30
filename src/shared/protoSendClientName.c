
#include "protoSendClientName.h"
#include "protoSendCommand.h"
#include "errorCodes.h" /* proto_err_t */
#include "protocol.h"
#include "user.h"

proto_err_t protoSendClientName(user_t *user, char *name, size_t name_length)
{
    return protoSendCommand(user, CMD_CLIENT_RESPONSE_NAME, name, name_length);
}