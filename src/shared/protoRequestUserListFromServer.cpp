
#include "protoRequestUserListFromServer.hpp"
#include "protoSendCommand.hpp"
#include "types.hpp"
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoRequestUserListFromServer(user_t *user)
{
    return protoSendCommand(
        user, CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER, NULL, 0);
}