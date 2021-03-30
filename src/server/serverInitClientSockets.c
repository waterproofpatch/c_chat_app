
#include "serverInitClientSockets.h"
#include "types.h"
#include "server.h"
#include "constants.h"

extern server_state_t g_server_state;

/**
 * @brief initialize the client sockets to NULL, the unused value.
 *
 */
void serverInitClientSockets()
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        g_server_state.client_sockets[i] = NULL;
    }
}