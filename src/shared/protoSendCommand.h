#ifndef _PROTO_SEND_COMMAND_H_
#define _PROTO_SEND_COMMAND_H_

#include <stdio.h> /* size_t */

#include "error_codes.h" /* proto_err_t */
#include "protocol.h"    /* command_type_t */
#include "user.h"

proto_err_t protoSendCommand(user_t *       user,
                             command_type_t cmd_type,
                             const char *   payload,
                             size_t         payload_length);

#endif