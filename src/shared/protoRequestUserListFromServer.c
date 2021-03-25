
#include "protoRequestUserListFromServer.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "error_codes.h"

proto_err_t protoRequestUserListFromServer(int sock_fd)
{
    return protoSendCommand(
        sock_fd, CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER, NULL, 0);
}