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


/**
 * @brief print a user object via list_foreach
 * 
 * @param user_opaque the user passed in via list_foreach
 * @param context context provided to list_foreach
 */
static void print_user(void *user_opaque, void *context)
{
    user_t *user = (user_t *)user_opaque;
    DBG_INFO("username %s, socket %d\n", user->name, user->client_socket_fd);
}

/**
 * @brief create a server
 * @param port_no: the port to listen on
 * @param sock_fd_out: ptr to socket to write with created server socket
 * @return: OK if server socket created successfully, ERR_* otherwise. See
 * error_codes.h.
 * */
static proto_err_t create_server(unsigned short port_no, int *sock_fd_out);

int    g_client_sockets[MAX_CLIENTS];   // each client's fd
int    g_max_fd;                        // max file descriptor
int    g_server_sock_fd = 0;            // server socket
fd_set g_all_fds;                       // set file descriptors

static char server_client_socket_fd_comparator(void *context, void *key)
{
    user_t *user    = (user_t *)context;
    int     sock_fd = *(int *)key;
    return user->client_socket_fd == sock_fd;
}

static void server_init_client_sockets()
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        g_client_sockets[i] = -1;
    }
}

static void server_add_client_socket(int socket_fd)
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_client_sockets[i] == -1)
        {
            g_client_sockets[i] = socket_fd;
            break;
        }
    }
}

static proto_err_t create_server(unsigned short port_no, int *sock_fd_out)
{
    int                sock_fd   = 0;
    struct sockaddr_in serv_addr = {0};

    // create an 'INTERNET' STREAM socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    *sock_fd_out = sock_fd;
    return OK;
}

static proto_err_t process_new_client(int g_server_sock_fd, list_t *active_user_list)
{
    proto_err_t        status         = OK;
    socklen_t          client_length  = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address = {0};
    int                new_sock_fd    = 0;

    new_sock_fd = accept(g_server_sock_fd, (struct sockaddr *)&client_address,
                         &client_length);
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
        close(new_sock_fd);
        status = ERR_GENERAL;
        goto done;
    }

    // get the name from the client
    char *name_out = NULL;
    status         = proto_read_client_name(new_sock_fd, &name_out);
    if (status != OK)
    {
        DBG_ERROR("Unable to read client name: %s\n", PROTO_ERR_T_STRING[status]);
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

    user_t *new_user = malloc(sizeof(user_t));
    if (!new_user)
    {
        DBG_ERROR("Unable to allocate a user\n");
        free(name_out);
        close(new_sock_fd);
        status = ERR_NO_MEM;
        goto done;
    }
    memset(new_user->name, '\0', MAX_USER_NAME_LENGTH);
    new_user->client_socket_fd = new_sock_fd;
    memcpy(new_user->name, name_out, strlen(name_out));
    free(name_out);
    user_t *existing_user = (user_t *)list_search(
        active_user_list, user_comparator, new_user->name);
    if (existing_user)
    {
        DBG_INFO("User with name %s already exists, kicking\n", new_user->name);
        status =
            proto_disconnect_client(new_sock_fd, "username already exists");
        if (status != OK)
        {
            DBG_ERROR("Unable to kick user: %s\n", PROTO_ERR_T_STRING[status]);
        }
        free(new_user);
        status = ERR_GENERAL;
        goto done;
    }

    DBG_INFO("Added user %s, socket %d to active user list.\n", new_user->name,
           new_user->client_socket_fd);
    list_add(active_user_list, new_user);
    DBG_INFO("User list is now:\n");
    list_foreach(active_user_list, print_user, NULL);
    server_add_client_socket(new_user->client_socket_fd);
    status = OK;

done:
    return status;
}

/**
 * @brief update the max FD value.
 *
 */
static void update_max_fd()
{
    FD_ZERO(&g_all_fds);
    FD_SET(g_server_sock_fd, &g_all_fds);
    g_max_fd = g_server_sock_fd;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_client_sockets[i] > -1)
        {
            FD_SET(g_client_sockets[i], &g_all_fds);
            g_max_fd = MAX(g_max_fd, g_client_sockets[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    // we'll listen on a high port to avoid having to sudo
    proto_err_t    status           = OK;
    unsigned short port_no          = 5001;
    list_t *       active_user_list = 0;

    // init the socket set for the clients
    server_init_client_sockets();

    // init the client socket client socket handler
    // pthread_create(&g_client_handler_thread, NULL,
    // server_handle_client_thread, NULL);

    // create the server
    DBG_INFO("server running on port %d\n", port_no);
    status = create_server(port_no, &g_server_sock_fd);
    if (status != OK)
    {
        DBG_INFO("Unable to create server: %s\n", PROTO_ERR_T_STRING[status]);
        return -1;
    }

    // initialize a user list to contain list of active users
    active_user_list = list_init(malloc, free);

    // wait for a client to connect
    int connected_clients = 0;

    while (1)
    {
        // recalculate the maximum FD for our call to select
        update_max_fd();

        int num_ready_fd = select(g_max_fd + 1, &g_all_fds, NULL, NULL, NULL);
        if (num_ready_fd < 0 && errno != EINTR)
        {
            DBG_INFO("Select error %d\n", errno);
            continue;
        }

        // TODO maybe need while num_ready_fd here, so we service each ready fd
        // before resetting them all in update_max_fd

        // if the activity was on our listening socket, it's a new connection
        if (FD_ISSET(g_server_sock_fd, &g_all_fds))
        {
            if (connected_clients == MAX_CLIENTS)
            {
                DBG_INFO("Cannot accept any new clients!\n");
                continue;
            }
            else
            {
                DBG_INFO("Processing new connection...\n");
                if (process_new_client(g_server_sock_fd, active_user_list) ==
                    OK)
                {
                    connected_clients++;
                }
                else
                {
                    DBG_INFO("Error processing new connection...\n");
                }
                continue;
            }
        }

        // otherwise we have a message from an already connected client
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = g_client_sockets[i];
            // if we don't have any stored fds or if the stored fd isn't the one
            // who woke us up, continue
            if (sd == -1 || !FD_ISSET(sd, &g_all_fds))
            {
                continue;
            }

            DBG_INFO("searching user list:\n");
            list_foreach(active_user_list, print_user, NULL);
            user_t *user = list_search(active_user_list,
                                       server_client_socket_fd_comparator, &sd);
            if (!user)
            {
                DBG_INFO(
                    "Someone is talking on socket %d, but I don't know "
                    "who...\n",
                    sd);
                return -1;   // this is a problem
            }
            DBG_INFO("User [%s] has a message for the server.\n", user->name);

            command_t * cmd    = NULL;
            proto_err_t status = OK;
            status             = proto_read_command(sd, &cmd);
            if (status != OK || cmd->command_type == CMD_REQUEST_DISCONNECT)
            {
                DBG_INFO("Status is %s, disconnecting client %s\n",
                       PROTO_ERR_T_STRING[status], user->name);
                g_client_sockets[i] = -1;
                connected_clients--;
                list_remove(active_user_list, user);
                close(sd);
                continue;
            }
            else if (cmd->command_type == CMD_REQUEST_USERLIST_FROM_SERVER)
            {
                DBG_INFO("User [%s] requests user list from server...\n",
                       user->name);
                proto_send_user_list(sd, active_user_list);
            }
            else
            {
                DBG_INFO("User [%s] send a command:\n", user->name);
                proto_print_command(cmd);
            }
        }
        DBG_INFO("Done\n");
    }
    return 0;
}
