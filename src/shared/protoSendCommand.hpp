#ifndef _PROTO_SEND_COMMAND_H_
#define _PROTO_SEND_COMMAND_H_

#include <stdio.h> /* size_t */

#include "errorCodes.hpp" /* proto_err_t */
#include "types.hpp"      /* command_type_t */
#include "user.hpp"

proto_err_t protoSendCommand(user_t *       user,
                             command_type_t cmd_type,
                             const char *   payload,
                             size_t         payload_length);

#endif