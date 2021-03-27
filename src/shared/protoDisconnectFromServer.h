
#ifndef _PROTO_DISCONNECT_FROM_SERVER_H_
#define _PROTO_DISCONNECT_FROM_SERVER_H_

#include "error_codes.h" /* proto_err_t */

proto_err_t protoDisconnectFromServer(int sock_fd, const char *reason);

#endif