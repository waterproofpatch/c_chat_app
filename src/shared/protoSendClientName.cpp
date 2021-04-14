
#include "protoSendClientName.hpp"
#include "protoSendCommand.hpp"
#include "errorCodes.hpp" /* proto_err_t */
#include "types.hpp"
#include "user.hpp"

proto_err_t protoSendClientName(user_t *user, char *name, size_t name_length)
{
    return protoSendCommand(user, CMD_CLIENT_RESPONSE_NAME, name, name_length);
}