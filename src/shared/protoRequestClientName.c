
#include "protoRequestClientName.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "error_codes.h"

proto_err_t protoRequestClientName(int sock_fd)
{
    return protoSendCommand(sock_fd, CMD_SERVER_REQUEST_NAME, NULL, 0);
}