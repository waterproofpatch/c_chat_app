
#include <stdio.h> /* size_t */

#include "protoBroadcastMessage.h"
#include "protoSendCommand.h"
#include "protocol.h"
#include "wrappers.h"
#include "error_codes.h"
#include "user.h"

proto_err_t protoBroadcastMessage(user_t *user,
                                  char *  name,
                                  size_t  name_length,
                                  char *  message,
                                  size_t  message_length)
{
    if (message_length > MAX_MESSAGE_LENGTH)
    {
        return ERR_PAYLOAD_TOO_LARGE;
    }
    broadcast_message_t *broadcast_message =
        wrappers_malloc(sizeof(broadcast_message_t) + message_length + 1);

    if (!broadcast_message)
    {
        return ERR_NO_MEM;
    }
    wrappers_memset(
        broadcast_message, 0, sizeof(broadcast_message_t) + message_length + 1);
    wrappers_memset(broadcast_message->name, '\0', MAX_USER_NAME_LENGTH);
    wrappers_memset(broadcast_message->message, '\0', message_length + 1);
    wrappers_memcpy(broadcast_message->name, name, name_length);
    wrappers_memcpy(broadcast_message->message, message, message_length);
    broadcast_message->message_length = message_length;
    proto_err_t res =
        protoSendCommand(user,
                         CMD_SERVER_BROADCAST_MESSAGE,
                         (char *)broadcast_message,
                         sizeof(broadcast_message_t) + message_length + 1);
    wrappers_free(broadcast_message);
    return res;
}