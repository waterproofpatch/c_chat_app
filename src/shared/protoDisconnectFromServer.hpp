
#ifndef _PROTO_DISCONNECT_FROM_SERVER_H_
#define _PROTO_DISCONNECT_FROM_SERVER_H_

#include "errorCodes.hpp" /* proto_err_t */
#include "user.hpp"

proto_err_t protoDisconnectFromServer(user_t *user, const char *reason);

#endif