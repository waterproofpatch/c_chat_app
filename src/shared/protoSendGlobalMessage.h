
#ifndef _PROTO_SEND_GLOBAL_MESSAGE_H_
#define _PROTO_SEND_GLOBAL_MESSAGE_H_

#include "error_codes.h"
#include "user.h"

proto_err_t protoSendGlobalMessage(user_t *user, char *buffer);

#endif