
#ifndef _PROTO_SEND_GLOBAL_MESSAGE_H_
#define _PROTO_SEND_GLOBAL_MESSAGE_H_

#include "error_codes.h"

proto_err_t protoSendGlobalMessage(int sock_fd, char *buffer);

#endif