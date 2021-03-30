
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "serverInitClientSockets.h"
#include "server.h"
#include "serverCreate.h"
#include "wrappers.h"
#include "errorCodes.h"
#include "list.h"

extern server_state_t g_server_state;

proto_err_t serverCreate(unsigned short port_no)
{
    int                sock_fd   = 0;
    struct sockaddr_in serv_addr = {0};

    serverInitClientSockets();

    // initialize a user list to contain list of active users
    g_server_state.active_user_list = list_init(wrappers_malloc, wrappers_free);
    if (!g_server_state.active_user_list)
    {
        DBG_ERROR("Failed initing user list.\n");
        return ERR_NO_MEM;
    }

    // create an 'INTERNET' STREAM socket
    sock_fd = wrappers_socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        DBG_ERROR("ERROR opening socket\n");
        return ERR_NETWORK_FAILURE;
    }

    // init socket structure
    wrappers_memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(port_no);

    int enable = 1;
    if (wrappers_setsockopt(
            sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        DBG_ERROR("Error setting sockopts.\n");
    }
    if (wrappers_setsockopt(
            sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    {
        DBG_ERROR("Error setting sockopts.\n");
    }

    // bind our socket file descriptor to the host address so it is notified of
    // inbound traffic/events...
    if (wrappers_bind(
            sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        DBG_ERROR("ERROR on binding: %s\n", strerror(errno));
        return ERR_NETWORK_FAILURE;
    }

    // listen on the socket
    if (wrappers_listen(sock_fd, 5) < 0)
    {
        DBG_ERROR("ERROR listening\n");
        return ERR_NETWORK_FAILURE;
    }

    g_server_state.server_sock_fd = sock_fd;
    return OK;
}