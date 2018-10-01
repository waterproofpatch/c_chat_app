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

pthread_t    g_receive_thread;   // thread to handle receiving data
volatile int g_is_alive =
    1;   // global to keep listen thread alive, volatile to ensure cache flush

// shameless SO theft of decent getline function...
static int get_line_from_user(char *buff, size_t buff_length);

// cmd loop to read input
static void cmd_loop(int sock_fd);

static int get_line_from_user(char *buff, size_t buff_length)
{
    int ch, extra;

    // get line with buffer overrun protection.
    printf("%s", "> ");
    fflush(stdout);
    if (fgets(buff, buff_length, stdin) == NULL)
    {
        return 0;
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

static void print_commands() {
    printf("Available commands:\n");
    printf("/getusers - Get list of conneted users\n");
    printf("/quit - Disconnect from server\n");
    printf("@<username> - PM a user\n");
}
static void cmd_loop(int sock_fd)
{
    print_commands();
    while (1)
    {
        char        buffer[256];
        proto_err_t status = OK;
        if (get_line_from_user(buffer, sizeof(buffer)) != 1)
        {
            printf("Unable to handle input.\n");
        }
        // disconect from the server
        if (strcmp(buffer, "/quit") == 0)
        {
            printf("Goodbye!\n");
            g_is_alive = 0;
            status = proto_disconnect_from_server(sock_fd,
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
            status = proto_request_userlist_from_server(sock_fd);
            if (status != OK)
            {
                printf("Unable to get userlist from server: %s\n",
                       PROTO_ERR_T_STRING[status]);
            }
        }
    }
}

int main(int argc, char **argv)
{
    printf("Starting client...\n");

    if (argc < 2)
    {
        printf("Usage: ./client <username>\n");
        return -1;
    }
    int                port_no        = 5001;
    int                sock_fd        = 0;
    char               hostname[]     = "127.0.0.1";
    struct sockaddr_in server_address = {0};

    // initialize the server address structure
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(port_no);
    if (inet_pton(AF_INET, hostname, &server_address.sin_addr) <= 0)
    {
        printf("inet_pton error occured: %s\n", strerror(errno));
        return -1;
    }

    // create a socket to handle data
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("ERROR opening socket\n");
        return -1;
    }

    // connect to the server address using the socket we created
    if (connect(sock_fd, (struct sockaddr *)&server_address,
                sizeof(struct sockaddr_in)) < 0)
    {
        printf("Error connecting: %s\n", strerror(errno));
        return -1;
    }

    // the server will tell us something...
    command_t cmd = {0};
    read(sock_fd, &cmd, sizeof(command_t));
    printf("Read command\n");
    proto_print_command(&cmd);
    if (cmd.command_type == CMD_REQUEST_NAME)
    {
        printf("Sending client name to server...\n");
        // proto_send_client_name(sock_fd, "somedumuser",
        // strlen("somedumbuser"));
        proto_send_client_name(sock_fd, argv[1], strlen(argv[1]));
    }

    // kick off receive thread to handle receipt of data
    pthread_create(&g_receive_thread, NULL, receive_function, &sock_fd);
    // enter command loop to process user input
    cmd_loop(sock_fd);
    return 0;
}

void *receive_function(void *context)
{
    int         sock_fd = *(int *)context;
    proto_err_t status  = OK;
    printf("Receive thread started...\n");
    while (g_is_alive)
    {
        command_t *cmd = NULL;
        status         = proto_read_command(sock_fd, &cmd);
        if (status != OK)
        {
            printf("Failed reading a command: %s\n",
                   PROTO_ERR_T_STRING[status]);
            continue;
        }
        proto_print_command(cmd);
        if (cmd->command_type == CMD_REQUEST_DISCONNECT)
        {
            printf(
                "Server requested that we disconnect for reason %s. "
                "Disconnecting...\n",
                cmd->payload);
            break;
        }
        else if (cmd->command_type == CMD_USER_LIST) {
            name_list_t* name_list = (name_list_t*)cmd->payload;
            printf("%d users connected:\n", name_list->num_names);
            for (int i = 0; i < name_list->num_names; i++) {
                printf("Name: [%s]\n", name_list->usernames[i]);
            }
        }
    }
    printf("Receive thread terminating.\n");
    return NULL;
}
