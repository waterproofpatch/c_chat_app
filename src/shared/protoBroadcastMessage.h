#ifndef _PROTO_BROADCAST_MESSAGE_H_
#define _PROTO_BROADCAST_MESSAGE_H_

#include <stdio.h> /* size_t */
#include "error_codes.h"

proto_err_t protoBroadcastMessage(int    sock_fd,
                                  char * name,
                                  size_t name_length,
                                  char * message,
                                  size_t message_length);
#endif