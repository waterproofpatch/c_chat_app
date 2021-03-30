
#include "protoRequestUserListFromServer.h"
#include "protoSendCommand.h"
#include "types.h"
#include "errorCodes.h"
#include "user.h"

proto_err_t protoRequestUserListFromServer(user_t *user)
{
    return protoSendCommand(
        user, CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER, NULL, 0);
}