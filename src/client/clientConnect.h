
#ifndef _CLIENT_CONNECT_H_
#define _CLIENT_CONNECT_H_

#include "error_codes.h"

proto_err_t clientConnect(char *hostname, unsigned short port_no);
#endif