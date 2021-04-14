

#ifndef _PROTO_DISCONNECT_CLIENT_H_
#define _PROTO_DISCONNECT_CLIENT_H_

#include "errorCodes.hpp" /* proto_err_t */
#include "user.hpp"

proto_err_t protoDisconnectClient(user_t *user, const char *reason);

#endif