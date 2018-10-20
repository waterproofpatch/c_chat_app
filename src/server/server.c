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
#include "error_codes.h"
#include "list.h"
#include "protocol.h"
#include "server.h"
#include "wrappers.h"

/**
 * @brief global state
 *
 */
server_state_t g_server_state = {0};

/**
 * @brief print a user object via list_foreach
 *
 * @param user_opaque the user passed in via list_foreach
 * @param context context provided to list_foreach
 */
static char server_client_socket_fd_comparator(void *context, void *key)
{
    user_t *user    = (user_t *)context;
    int     sock_fd = *(int *)key;
    return user->client_socket_fd == sock_fd;
}

/**
 * @brief get the file descriptor for client at index
 *
 * @param index the index into the global file descriptors table
 * @return int the fd, or -1 if not found/not init
 */
static int server_get_client_socket(int index)
{
    if (index >= MAX_CLIENTS)
    {
        return -1;
    }
    return g_server_state.client_sockets[index];
}

/**
 * @brief initialize the client sockets to -1, the unused value.
 *
 */
static void server_init_client_sockets()
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        g_server_state.client_sockets[i] = -1;
    }
}

/**
 * @brief remove a client from the server
 *
 * @param user the user to remove
 */
static void server_remove_user(user_t *user)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] == user->client_socket_fd)
        {
            g_server_state.client_sockets[i] = -1;
            break;
        }
    }
    wrappers_close(user->client_socket_fd);
    g_server_state.connected_clients--;
    list_remove(g_server_state.active_user_list, user);
    wrappers_free(user);
}

/**
 * @brief register a user with the server
 *
 * @param user the user to register
 */
static void server_add_user(user_t *user)
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] == -1)
        {
            g_server_state.client_sockets[i] = user->client_socket_fd;
            break;
        }
    }
    g_server_state.connected_clients++;
    list_add(g_server_state.active_user_list, user);
}

/**
 * @brief process a new client connection to the server socket
 *
 * @return proto_err_t OK if client is accepted, ERR_* otherwise
 */
static proto_err_t process_new_client()
{
    proto_err_t status =
        OK;   // if we return anything other than OK, no user is accepted
    socklen_t client_length =
        sizeof(struct sockaddr_in);            // for the client socket
    struct sockaddr_in client_address = {0};   // the remote address of the
                                               // client
    int   new_sock_fd = 0;                     // new socket for the user
    char *name_out = NULL;   // allocated by proto when we read in the username
    user_t *new_user = NULL;   // created when we get the username

    new_sock_fd = accept(g_server_state.server_sock_fd,
                         (struct sockaddr *)&client_address, &client_length);
    if (new_sock_fd < 0)
    {
        DBG_ERROR("ERROR on accept\n");
        return ERR_NETWORK_FAILURE;
    }
    DBG_INFO("Got a new client, asking for name!\n");

    // ask the client for their name
    status = proto_request_client_name(new_sock_fd);
    if (status != OK)
    {
        DBG_ERROR("Unable to request client name: %s\n",
                  PROTO_ERR_T_STRING[status]);
        status = ERR_GENERAL;
        goto done;
    }

    // get the name from the client
    status = proto_read_client_name(new_sock_fd, &name_out);
    if (status != OK)
    {
        DBG_ERROR("Unable to read client name: %s\n",
                  PROTO_ERR_T_STRING[status]);
        status = proto_disconnect_client(new_sock_fd, "invalid client name");
        if (status != OK)
        {
            DBG_ERROR("Unable to disconnect client: %s\n",
                      PROTO_ERR_T_STRING[status]);
            goto done;
        }
        status = ERR_GENERAL;
        goto done;
    }

    new_user = wrappers_malloc(sizeof(user_t));
    if (!new_user)
    {
        DBG_ERROR("Unable to allocate a user\n");
        status = ERR_NO_MEM;
        goto done;
    }
    new_user->client_socket_fd = new_sock_fd;
    memset(new_user->name, '\0', MAX_USER_NAME_LENGTH);
    memcpy(new_user->name, name_out, strlen(name_out));

    // check if user exists
    user_t *existing_user = (user_t *)list_search(
        g_server_state.active_user_list, user_comparator, new_user->name);
    if (existing_user)
    {
        DBG_INFO("User with name %s already exists, kicking\n", new_user->name);
        status =
            proto_disconnect_client(new_sock_fd, "username already exists");
        if (status != OK)
        {
            DBG_ERROR("Unable to kick user: %s\n", PROTO_ERR_T_STRING[status]);
        }
        status = ERR_GENERAL;
        goto done;
    }

    DBG_INFO("Added user %s, socket %d to active user list.\n", new_user->name,
             new_user->client_socket_fd);
    server_add_user(new_user);
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

/**
 * @brief update the max FD value.
 *
 */
static void server_update_max_fd()
{
    FD_ZERO(&g_server_state.all_fds);
    FD_SET(g_server_state.server_sock_fd, &g_server_state.all_fds);
    g_server_state.max_fd = g_server_state.server_sock_fd;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_server_state.client_sockets[i] > -1)
        {
            FD_SET(g_server_state.client_sockets[i], &g_server_state.all_fds);
            g_server_state.max_fd =
                MAX(g_server_state.max_fd, g_server_state.client_sockets[i]);
        }
    }
}

void server_handle_connections()
{
    server_update_max_fd();

    int num_ready_fd = wrappers_select(
        g_server_state.max_fd + 1, &g_server_state.all_fds, NULL, NULL, NULL);
    if (num_ready_fd < 0 && errno != EINTR)
    {
        DBG_INFO("Select error %d\n", errno);
        return;
    }

    // if the activity was on our listening socket, it's a new connection
    if (FD_ISSET(g_server_state.server_sock_fd, &g_server_state.all_fds))
    {
        if (g_server_state.connected_clients == MAX_CLIENTS)
        {
            DBG_INFO("Cannot accept any new clients!\n");
            return;
        }
        else
        {
            DBG_INFO("Processing new connection...\n");
            if (process_new_client() == OK)
            {
                DBG_INFO("Accepted new client!\n");
            }
            else
            {
                DBG_INFO("Error processing new connection...\n");
            }
            return;
        }
    }

    // otherwise we have a message from an already connected client
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        int sd = server_get_client_socket(i);
        // if we don't have any stored fds or if the stored fd isn't the one
        // who woke us up, continue
        if (sd == -1 || !FD_ISSET(sd, &g_server_state.all_fds))
        {
            continue;
        }

        user_t *user = list_search(g_server_state.active_user_list,
                                   server_client_socket_fd_comparator, &sd);
        if (!user)
        {
            DBG_INFO(
                "Someone is talking on socket %d, but I don't know "
                "who...\n",
                sd);
            continue;   // this is a problem
        }
        DBG_INFO("User [%s] has a message for the server.\n", user->name);

        command_t * cmd    = NULL;
        proto_err_t status = OK;
        status             = proto_read_command(sd, &cmd);
        if (status != OK || cmd->command_type == CMD_SHARED_REQUEST_DISCONNECT)
        {
            DBG_INFO("Status is %s, disconnecting client %s\n",
                     PROTO_ERR_T_STRING[status], user->name);
            server_remove_user(user);
        }
        else if (cmd->command_type == CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER)
        {
            DBG_INFO("User [%s] requests user list from server...\n",
                     user->name);
            proto_send_user_list(sd, g_server_state.active_user_list);
        }
        else if (cmd->command_type == CMD_CLIENT_BROADCAST_MESSAGE)
        {
            DBG_INFO("User %s sent a message to everyone: %s\n", user->name,
                     cmd->payload);
            // send a message to *each* client
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (server_get_client_socket(j) >= 0)
                {
                    proto_broadcast_message(server_get_client_socket(j),
                                            user->name, strlen(user->name),
                                            cmd->payload, strlen(cmd->payload));
                }
            }
        }
        else
        {
            DBG_INFO("User [%s] send a command:\n", user->name);
            proto_print_command(cmd);
        }
    }
    return;
}

proto_err_t server_create(unsigned short port_no)
{
    int                sock_fd   = 0;
    struct sockaddr_in serv_addr = {0};

    server_init_client_sockets();

    // initialize a user list to contain list of active users
    g_server_state.active_user_list = list_init(wrappers_malloc, wrappers_free);

    // create an 'INTERNET' STREAM socket
    sock_fd = wrappers_socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        DBG_ERROR("ERROR opening socket\n");
        return ERR_NETWORK_FAILURE;
    }

    // init socket structure
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(port_no);

    int enable = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        DBG_ERROR("Error setting sockopts.\n");
    }
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
    {
        DBG_ERROR("Error setting sockopts.\n");
    }

    // bind our socket file descriptor to the host address so it is notified of
    // inbound traffic/events...
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        DBG_ERROR("ERROR on binding: %s\n", strerror(errno));
        return ERR_NETWORK_FAILURE;
    }

    // listen on the socket
    if (listen(sock_fd, 5) < 0)
    {
        DBG_ERROR("ERROR listening\n");
        return ERR_NETWORK_FAILURE;
    }

    g_server_state.server_sock_fd = sock_fd;
    return OK;
}
