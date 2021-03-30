
#ifndef _PROTO_SEND_GLOBAL_MESSAGE_H_
#define _PROTO_SEND_GLOBAL_MESSAGE_H_

#include "errorCodes.h"
#include "user.h"

proto_err_t protoSendGlobalMessage(user_t *user, char *buffer);

#endif