
#ifndef _PROTO_READ_CLIENT_NAME_H_
#define _PROTO_READ_CLIENT_NAME_H_

#include "errorCodes.hpp" /* proto_err_t */
#include "user.hpp"

proto_err_t protoReadClientName(user_t *user, char **name_out);

#endif