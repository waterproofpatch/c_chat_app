#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>

#include "user.h"
#include "errorCodes.h"
#include "list.h"
#include "protocol.h"
#include "server.h"
#include "wrappers.h"

/**
 * @brief global state
 *
 */
server_state_t g_server_state = {0};
