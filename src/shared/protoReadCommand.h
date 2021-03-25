
#ifndef _PROTO_READ_COMMAND_H_
#define _PROTO_READ_COMMAND_H_

#include "protocol.h"
#include "error_codes.h"

proto_err_t protoReadCommand(int sock_fd, command_t **cmd_out);

#endif