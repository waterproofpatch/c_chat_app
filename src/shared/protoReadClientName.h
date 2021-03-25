
#ifndef _PROTO_READ_CLIENT_NAME_H_
#define _PROTO_READ_CLIENT_NAME_H_

#include "error_codes.h" /* proto_err_t */

proto_err_t protoReadClientName(int sock_fd, char **name_out);

#endif