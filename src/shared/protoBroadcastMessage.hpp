#ifndef _PROTO_BROADCAST_MESSAGE_H_
#define _PROTO_BROADCAST_MESSAGE_H_

#include <stdio.h> /* size_t */
#include "errorCodes.hpp"
#include "user.hpp"

proto_err_t protoBroadcastMessage(user_t *user,
                                  char *  name,
                                  size_t  name_length,
                                  char *  message,
                                  size_t  message_length);
#endif