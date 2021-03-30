

#ifndef _PROTO_DISCONNECT_CLIENT_H_
#define _PROTO_DISCONNECT_CLIENT_H_

#include "errorCodes.h" /* proto_err_t */
#include "user.h"

proto_err_t protoDisconnectClient(user_t *user, const char *reason);

#endif