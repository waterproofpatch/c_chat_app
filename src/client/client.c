#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "protocol.h"
#include "user.h"
#include "client.h"
#include "wrappers.h"

int          g_sock_fd;          // socket descriptor for the client connection
pthread_t    g_receive_thread;   // thread to handle receiving data
volatile int g_is_alive =
    1;   // global to keep listen thread alive, volatile to ensure cache flush

/**
 * @brief shameless SO copy pasta to read a line from stdin
 *
 * @param buff buffer to fill with user data
 * @param buff_length length of the buffer
 * @return int 1 on success, -1 on failure
 */
static int client_get_line_from_user(char *buff, size_t buff_length)
{
    int ch, extra;

    // get line with buffer overrun protection.
    printf("%s", "> ");
    fflush(stdout);
    if (fgets(buff, buff_length, stdin) == NULL)
    {
        return -1;
    }

    // if it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff) - 1] != '\n')
    {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
        {
            extra = 1;
        }
        return (extra == 1) ? -1 : 1;
    }

    // otherwise remove newline and give string back to caller.
    buff[strlen(buff) - 1] = '\0';
    return 1;
}

/**
 * @brief print the available commands
 *
 */
static void client_print_commands()
{
    printf("Available commands:\n");
    printf("/getusers - Get list of conneted users\n");
    printf("/quit - Disconnect from server\n");
    printf("@<username> - PM a user\n");
}

proto_err_t client_handshake(char *username)
{
    // the server will tell us something...
    command_t cmd = {0};
    read(g_sock_fd, &cmd, sizeof(command_t));
    if (cmd.command_type == CMD_SERVER_REQUEST_NAME)
    {
        printf("Sending client name to server...\n");
        // proto_send_client_name(sock_fd, "somedumuser",
        // strlen("somedumbuser"));
        proto_send_client_name(g_sock_fd, username, strlen(username));
    }
    else
    {
        printf("Server asked for something other than a name\n");
        return ERR_INVALID_COMMAND;
    }

    // kick off receive thread to handle receipt of data
    pthread_create(&g_receive_thread, NULL, client_receive_function,
                   &g_sock_fd);
    return OK;
}

proto_err_t client_connect(char *hostname, unsigned short port_no)
{
    struct sockaddr_in server_address = {0};

    // initialize the server address structure
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, hostname, &server_address.sin_addr) <= 0)
    {
        printf("inet_pton error occured: %s\n", strerror(errno));
        return ERR_GENERAL;
    }

    // create a socket to handle data
    g_sock_fd = wrappers_socket(AF_INET, SOCK_STREAM, 0);
    if (g_sock_fd < 0)
    {
        printf("ERROR opening socket\n");
        return ERR_NETWORK_FAILURE;
    }

    // connect to the server address using the socket we created TODO use
    // wrappers
    if (connect(g_sock_fd, (struct sockaddr *)&server_address,
                sizeof(struct sockaddr_in)) < 0)
    {
        printf("Error connecting: %s\n", strerror(errno));
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}

proto_err_t client_loop()
{
    client_print_commands();
    while (1)
    {
        char        buffer[256];
        proto_err_t status = OK;
        if (client_get_line_from_user(buffer, sizeof(buffer)) != 1)
        {
            printf("Unable to handle input.\n");
        }
        // disconect from the server
        if (strcmp(buffer, "/quit") == 0)
        {
            printf("Goodbye!\n");
            g_is_alive = 0;
            status     = proto_disconnect_from_server(g_sock_fd,
                                                  "user initiated disconnect");
            if (status != OK)
            {
                printf("Unable to disconnect from server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
            pthread_cancel(g_receive_thread);
            printf("Joining receive thread...\n");
            pthread_join(g_receive_thread, NULL);
            printf("Joined receive thread, terminating cmd loop\n");
            break;
        }
        // get a list of users connected to the server
        if (strcmp(buffer, "/getusers") == 0)
        {
            printf("Requesting user list from server\n");
            status = proto_request_userlist_from_server(g_sock_fd);
            if (status != OK)
            {
                printf("Unable to get userlist from server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
        }
        else
        {
            status = proto_send_global_message(g_sock_fd, buffer);
            if (status != OK)
            {
                printf("Unable to get send message to server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
        }
    }
    return OK;
}

void *client_receive_function(void *context)
{
    int         sock_fd = *(int *)context;
    proto_err_t status  = OK;
    command_t * cmd     = NULL;
    printf("Receive thread started...\n");
    while (g_is_alive)
    {
        // free any previously allocated command
        if (cmd)
        {
            wrappers_free(cmd);
            cmd = NULL;
        }
        status = proto_read_command(sock_fd, &cmd);
        if (status != OK)
        {
            printf("Failed reading a command: %s\n",
                   PROTO_ERR_T_STRING[status]);
            if (status == ERR_REMOTE_HOST_CLOSED)
            {
                printf("Cannot recover from this error. Closing down...\n");
                break;
            }
            continue;
        }
        name_list_t *        name_list         = NULL;
        broadcast_message_t *broadcast_message = NULL;
        switch (cmd->command_type)
        {
            case CMD_SHARED_REQUEST_DISCONNECT:
                printf(
                    "Server requested that we disconnect for reason %s. "
                    "Disconnecting...\n",
                    cmd->payload);
                break;
            case CMD_SERVER_USER_LIST:
                name_list = (name_list_t *)cmd->payload;
                printf("%d users connected:\n", name_list->num_names);
                for (int i = 0; i < name_list->num_names; i++)
                {
                    printf("Name: [%s]\n", name_list->usernames[i]);
                }
                break;
            case CMD_SERVER_BROADCAST_MESSAGE:
                broadcast_message = (broadcast_message_t *)cmd->payload;
                printf("%s: %s\n", broadcast_message->name,
                       broadcast_message->message);
                break;
            default:
                printf("unknown command %d\n", cmd->command_type);
                break;
        }
    }   // end while alive loop
    printf("Receive thread terminating.\n");
    if (cmd)
    {
        wrappers_free(cmd);
        cmd = NULL;
    }
    return NULL;
}
