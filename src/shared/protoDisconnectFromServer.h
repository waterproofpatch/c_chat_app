
#ifndef _PROTO_DISCONNECT_FROM_SERVER_H_
#define _PROTO_DISCONNECT_FROM_SERVER_H_

#include "error_codes.h" /* proto_err_t */
#include "user.h"

proto_err_t protoDisconnectFromServer(user_t *user, const char *reason);

#endif