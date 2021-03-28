
#ifndef _PROTO_READ_CLIENT_NAME_H_
#define _PROTO_READ_CLIENT_NAME_H_

#include "error_codes.h" /* proto_err_t */
#include "user.h"

proto_err_t protoReadClientName(user_t *user, char **name_out);

#endif