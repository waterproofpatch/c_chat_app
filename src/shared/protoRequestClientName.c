
#include "protoRequestClientName.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "errorCodes.h"
#include "user.h"

proto_err_t protoRequestClientName(user_t *user)
{
    return protoSendCommand(user, CMD_SERVER_REQUEST_NAME, NULL, 0);
}