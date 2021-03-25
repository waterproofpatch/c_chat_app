
#ifndef _PROTO_SEND_USER_LIST_H_
#define _PROTO_SEND_USER_LIST_H_

#include "list.h"
#include "error_codes.h"

proto_err_t protoSendUserList(int sock_fd, list_t *user_list);

#endif