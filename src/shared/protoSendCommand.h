#ifndef _PROTO_SEND_COMMAND_H_
#define _PROTO_SEND_COMMAND_H_

#include <stdio.h> /* size_t */

#include "error_codes.h" /* proto_err_t */
#include "protocol.h"    /* command_type_t */

proto_err_t protoSendCommand(int            sock_fd,
                             command_type_t cmd_type,
                             char *         payload,
                             size_t         payload_length);

#endif