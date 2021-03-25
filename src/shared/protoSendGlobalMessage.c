
#include <string.h>

#include "protoSendGlobalMessage.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "error_codes.h"

proto_err_t protoSendGlobalMessage(int sock_fd, char *buffer)
{
    return protoSendCommand(
        sock_fd, CMD_CLIENT_BROADCAST_MESSAGE, buffer, strlen(buffer));
}