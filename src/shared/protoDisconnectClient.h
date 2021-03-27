

#ifndef _PROTO_DISCONNECT_CLIENT_H_
#define _PROTO_DISCONNECT_CLIENT_H_

#include "error_codes.h" /* proto_err_t */

proto_err_t protoDisconnectClient(int sock_fd, const char *reason);

#endif