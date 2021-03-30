
#ifndef _PROTO_READ_COMMAND_H_
#define _PROTO_READ_COMMAND_H_

#include "protocol.h"
#include "errorCodes.h"
#include "user.h"

proto_err_t protoReadCommand(user_t *user, command_t **cmd_out);

#endif