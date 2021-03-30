#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "protoRequestClientName.h"
#include "protoReadClientName.h"
#include "protoDisconnectClient.h"
#include "serverProcessNewClient.h"
#include "serverConfigureSslForClient.h"
#include "serverAddUser.h"
#include "server.h"
#include "wrappers.h"
#include "debug.h"

extern server_state_t g_server_state;

/**
 * @brief process a new client connection to the server socket
 *
 * @return proto_err_t OK if client is accepted, ERR_* otherwise
 */
proto_err_t serverProcessNewClient()
{
    proto_err_t status = OK;
    socklen_t   client_length =
        sizeof(struct sockaddr_in);            // for the client socket
    struct sockaddr_in client_address = {0};   // the remote address of the
                                               // client
    int   new_sock_fd = 0;                     // new socket for the user
    char *name_out = NULL;   // allocated by proto when we read in the username
    user_t *new_user = NULL;   // created when we get the username

    new_sock_fd = accept(g_server_state.server_sock_fd,
                         (struct sockaddr *)&client_address,
                         &client_length);
    if (new_sock_fd < 0)
    {
        DBG_ERROR("ERROR on accept\n");
        return ERR_NETWORK_FAILURE;
    }

    new_user = wrappers_malloc(sizeof(user_t));
    if (!new_user)
    {
        DBG_ERROR("Unable to allocate a user\n");
        status = ERR_NO_MEM;
        goto done;
    }
    wrappers_memset(new_user, 0, sizeof(*new_user));

    new_user->client_socket_fd = new_sock_fd;

    if (serverConfigureSslForClient(new_user) != OK)
    {
        DBG_ERROR("Failed configuring SSL ctx for client...\n");
        status = ERR_SSL;
        goto done;
    }
    DBG_INFO("Got a new client, asking for name!\n");

    // ask the client for their name
    status = protoRequestClientName(new_user);
    if (status != OK)
    {
        DBG_ERROR("Unable to request client name: %s\n",
                  PROTO_ERR_T_STRING[status]);
        status = ERR_GENERAL;
        goto done;
    }

    // get the name from the client
    status = protoReadClientName(new_user, &name_out);
    if (status != OK)
    {
        DBG_ERROR("Unable to read client name: %s\n",
                  PROTO_ERR_T_STRING[status]);
        status = protoDisconnectClient(new_user, "invalid client name");
        if (status != OK)
        {
            DBG_ERROR("Unable to disconnect client: %s\n",
                      PROTO_ERR_T_STRING[status]);
            goto done;
        }
        status = ERR_GENERAL;
        goto done;
    }

    memset(new_user->name, '\0', MAX_USER_NAME_LENGTH);
    memcpy(new_user->name, name_out, strlen(name_out));

    // check if user exists
    user_t *existing_user = (user_t *)list_search(
        g_server_state.active_user_list, user_comparator, new_user->name);
    if (existing_user)
    {
        DBG_INFO("User with name %s already exists, kicking\n", new_user->name);
        status = protoDisconnectClient(new_user, "username already exists");
        if (status != OK)
        {
            DBG_ERROR("Unable to kick user: %s\n", PROTO_ERR_T_STRING[status]);
        }
        status = ERR_GENERAL;
        goto done;
    }

    DBG_INFO("Added user %s, socket %d to active user list.\n",
             new_user->name,
             new_user->client_socket_fd);
    serverAddUser(new_user);
    status = OK;

done:
    if (name_out)
    {
        wrappers_free(name_out);
        name_out = NULL;
    }
    if (OK != status)
    {
        if (new_user)
        {
            wrappers_free(new_user);
            new_user = NULL;
        }
        wrappers_close(new_sock_fd);
    }
    return status;
}