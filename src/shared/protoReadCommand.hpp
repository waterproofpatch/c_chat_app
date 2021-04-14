
#ifndef _PROTO_READ_COMMAND_H_
#define _PROTO_READ_COMMAND_H_

#include "types.hpp"
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoReadCommand(user_t *user, command_t **cmd_out);

#endif