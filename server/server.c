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

proto_err_t create_server(unsigned short port_no, int *sock_fd_out);

pthread_t g_client_handler_thread;
int       g_client_sockets[MAX_CLIENTS];
fd_set    g_all_fds;
int       g_max_fd = 0;

char server_client_socket_fd_comparator(void *context, void *key)
{
    user_t *user    = (user_t *)context;
    int     sock_fd = *(int *)key;
    return user->client_socket_fd == sock_fd;
}

void server_init_client_sockets()
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        g_client_sockets[i] = 0;
    }
}

void server_add_client_socket(int socket_fd)
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_client_sockets[i] == 0)
        {
            g_client_sockets[i] = socket_fd;
            break;
        }
    }
}

proto_err_t create_server(unsigned short port_no, int *sock_fd_out)
{
    int                sock_fd   = 0;
    struct sockaddr_in serv_addr = {0};

    // create an 'INTERNET' STREAM socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("ERROR opening socket\n");
        return ERR_NETWORK_FAILURE;
    }

    // init socket structure
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(port_no);

    // bind our socket file descriptor to the host address so it is notified of
    // inbound traffic/events...
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR on binding: %s\n", strerror(errno));
        return ERR_NETWORK_FAILURE;
    }

    // listen on the socket
    if (listen(sock_fd, 5) < 0)
    {
        printf("ERROR listening\n");
        return ERR_NETWORK_FAILURE;
    }

    *sock_fd_out = sock_fd;
    return OK;
}

proto_err_t process_new_client(int server_sock_fd, list_t *active_user_list)
{
    proto_err_t        status         = OK;
    socklen_t          client_length  = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address = {0};
    int                new_sock_fd    = 0;

    new_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_address,
                         &client_length);
    if (new_sock_fd < 0)
    {
        printf("ERROR on accept\n");
        return ERR_NETWORK_FAILURE;
    }
    printf("Got a new client, asking for name!\n");

    // ask the client for their name
    status = proto_request_client_name(new_sock_fd);
    if (status != OK)
    {
        printf("Unable to request client name: %s\n",
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
        printf("Unable to read client name: %s\n", PROTO_ERR_T_STRING[status]);
        status = proto_disconnect_client(new_sock_fd, "invalid client name");
        if (status != OK)
        {
            printf("Unable to disconnect client: %s\n",
                   PROTO_ERR_T_STRING[status]);
            goto done;
        }
        status = ERR_GENERAL;
        goto done;
    }

    printf("Adding a user!\n");
    user_t *new_user = malloc(sizeof(user_t));
    if (!new_user)
    {
        printf("Unable to allocate a user\n");
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
        printf("User with name %s already exists, kicking\n", new_user->name);
        status =
            proto_disconnect_client(new_sock_fd, "username already exists");
        if (status != OK)
        {
            printf("Unable to kick user: %s\n", PROTO_ERR_T_STRING[status]);
        }
        free(new_user);
        status = ERR_GENERAL;
        goto done;
    }

    list_add(active_user_list, new_user);
    printf("Added user %s to active user list.\n", new_user->name);
    server_add_client_socket(new_user->client_socket_fd);
    // set the file descriptor
    FD_SET(new_user->client_socket_fd, &g_all_fds);
done:
    if (status != OK)
    {
        // clear the descriptor
        FD_CLR(new_sock_fd, &g_all_fds);
    }

    // calculate new max fd
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_client_sockets[i] > 0)
        {
            if (FD_ISSET(g_client_sockets[i], &g_all_fds))
            {
                g_max_fd = MAX(g_max_fd, g_client_sockets[i]);
            }
        }
    }
    return status;
}

int main(int argc, char *argv[])
{
    // we'll listen on a high port to avoid having to sudo
    proto_err_t    status           = OK;
    int            server_sock_fd   = 0;
    unsigned short port_no          = 5001;
    list_t *       active_user_list = 0;

    // init the socket set for the clients
    server_init_client_sockets();

    // init the client socket client socket handler
    // pthread_create(&g_client_handler_thread, NULL,
    // server_handle_client_thread, NULL);

    // create the server
    printf("server running on port %d\n", port_no);
    status = create_server(port_no, &server_sock_fd);
    if (status != OK)
    {
        printf("Unable to create server: %s\n", PROTO_ERR_T_STRING[status]);
        return -1;
    }

    // initialize a user list to contain list of active users
    active_user_list = list_init(malloc, free);

    // wait for a client to connect
    int connected_clients = 0;

    FD_ZERO(&g_all_fds);
    FD_SET(server_sock_fd, &g_all_fds);
    g_max_fd = server_sock_fd;

    while (1)
    {
        printf("blocking on select\n");
        int activity = select(g_max_fd + 1, &g_all_fds, NULL, NULL, NULL);
        printf("done blocking on select\n");
        if (activity < 0 && errno != EINTR)
        {
            printf("Select error %d\n", errno);
            continue;
        }

        // if the activity was on our listening socket, it's a new connection'
        if (FD_ISSET(server_sock_fd, &g_all_fds))
        {
            if (connected_clients == MAX_CLIENTS)
            {
                printf("Cannot accept any new clients!\n");
                continue;
            }
            else
            {
                printf("Processing new client...\n");
                if (process_new_client(server_sock_fd, active_user_list) == OK)
                {
                    connected_clients++;
                }
                continue;
            }
        }
        // otherwise we have a message from an already connected client
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = g_client_sockets[i];
            // if it's not this client, check the next one... and so on, for
            // each of them.
            if (!FD_ISSET(sd, &g_all_fds))
            {
                continue;
            }
            user_t *user = list_search(active_user_list,
                                       server_client_socket_fd_comparator, &sd);
            if (!user)
            {
                printf("Someone is talking, but I don't know who...\n");
                return -1;   // this is a problem
            }
            else
            {
                printf("User [%s] has a message for the server.\n", user->name);
            }
            command_t * cmd    = NULL;
            proto_err_t status = OK;
            status             = proto_read_command(sd, &cmd);
            if (status != OK)
            {
                printf("Unable to read command from client [%s]: %s\n",
                       user->name, PROTO_ERR_T_STRING[status]);
                g_client_sockets[i] = 0;
                connected_clients--;
                list_remove(active_user_list, user);
                FD_CLR(sd, &g_all_fds);
                close(sd);
                continue;
            }
            if (cmd->command_type == CMD_REQUEST_DISCONNECT)
            {
                printf("Client [%s] request disconnect.\n", user->name);
                g_client_sockets[i] = 0;
                connected_clients--;
                FD_CLR(sd, &g_all_fds);
                close(sd);
                list_remove(active_user_list, user);
                continue;
            }
            else if (cmd->command_type == CMD_REQUEST_USERLIST_FROM_SERVER)
            {
                printf("User [%s] requests user list from server...\n",
                       user->name);
                proto_send_user_list(sd, active_user_list);
            }
            else
            {
                printf("User [%s] send a command:\n", user->name);
                proto_print_command(cmd);
            }
        }
        printf("Done\n");
    }
    return 0;
}
