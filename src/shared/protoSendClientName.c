
#include "protoSendClientName.h"
#include "protoSendCommand.h"
#include "error_codes.h" /* proto_err_t */
#include "protocol.h"

proto_err_t protoSendClientName(int sock_fd, char *name, size_t name_length)
{
    return protoSendCommand(
        sock_fd, CMD_CLIENT_RESPONSE_NAME, name, name_length);
}