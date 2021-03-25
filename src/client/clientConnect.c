#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "clientConnect.h"
#include "error_codes.h"
#include "wrappers.h"

extern int g_sock_fd;   // socket descriptor for the client connection

proto_err_t clientConnect(char *hostname, unsigned short port_no)
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
    if (wrappers_connect(g_sock_fd,
                         (struct sockaddr *)&server_address,
                         sizeof(struct sockaddr_in)) < 0)
    {
        printf("Error connecting: %s\n", strerror(errno));
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}