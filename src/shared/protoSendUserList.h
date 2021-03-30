
#ifndef _PROTO_SEND_USER_LIST_H_
#define _PROTO_SEND_USER_LIST_H_

#include "list.h"
#include "errorCodes.h"
#include "user.h"

proto_err_t protoSendUserList(user_t *user, list_t *user_list);

#endif