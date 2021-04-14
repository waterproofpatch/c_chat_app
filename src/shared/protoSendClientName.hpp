
#ifndef _PROTO_SEND_CLIENT_NAME_H_
#define _PROTO_SEND_CLIENT_NAME_H_

#include "errorCodes.hpp" /* proto_err_t */
#include "user.hpp"

proto_err_t protoSendClientName(user_t *user, char *name, size_t name_length);

#endif