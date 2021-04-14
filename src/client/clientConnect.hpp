
#ifndef _CLIENT_CONNECT_H_
#define _CLIENT_CONNECT_H_

#include "errorCodes.hpp"

proto_err_t clientConnect(char *hostname, unsigned short port_no);
#endif