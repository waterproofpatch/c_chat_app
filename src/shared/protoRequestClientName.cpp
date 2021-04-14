
#include "protoRequestClientName.hpp"
#include "protoSendCommand.hpp"
#include "types.hpp"
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoRequestClientName(user_t *user)
{
    return protoSendCommand(user, CMD_SERVER_REQUEST_NAME, NULL, 0);
}