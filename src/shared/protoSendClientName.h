
#ifndef _PROTO_SEND_CLIENT_NAME_H_
#define _PROTO_SEND_CLIENT_NAME_H_

#include <stdio.h>

#include "error_codes.h" /* proto_err_t */

proto_err_t protoSendClientName(int sock_fd, char *name, size_t name_length);

#endif