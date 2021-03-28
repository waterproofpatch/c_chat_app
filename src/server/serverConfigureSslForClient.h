
#ifndef _SERVER_CONFIGURE_SSL_FOR_CLIENT_H_
#define _SERVER_CONFIGURE_SSL_FOR_CLIENT_H_

#include <openssl/ssl.h>

#include "user.h"
#include "protocol.h"

proto_err_t serverConfigureSslForClient(user_t *user);

#endif