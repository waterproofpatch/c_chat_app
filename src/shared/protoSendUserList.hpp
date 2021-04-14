
#ifndef _PROTO_SEND_USER_LIST_H_
#define _PROTO_SEND_USER_LIST_H_

#include "list.hpp"
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoSendUserList(user_t *user, list_t *user_list);

#endif