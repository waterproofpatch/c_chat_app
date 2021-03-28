
#ifndef _PROTO_SEND_CLIENT_NAME_H_
#define _PROTO_SEND_CLIENT_NAME_H_

#include <stdio.h>

#include "error_codes.h" /* proto_err_t */
#include "user.h"

proto_err_t protoSendClientName(user_t *user, char *name, size_t name_length);

#endif