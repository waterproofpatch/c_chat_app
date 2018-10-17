#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "error_codes.h"
#include "client.h"

int main(int argc, char **argv)
{
    printf("Starting client...\n");

    if (argc < 2)
    {
        printf("Usage: ./client <username>\n");
        return -1;
    }
    unsigned short  port_no    = 5001;
    char hostname[] = "127.0.0.1";

    proto_err_t res = OK;
    res             = client_connect(hostname, port_no);
    if (OK != res)
    {
        printf("Unable to connect: %s\n", PROTO_ERR_T_STRING[res]);
    }

    // send name to server
    res = client_handshake(argv[1]);
    if (OK != res)
    {
        printf("Unable to complete handshake: %s\n", PROTO_ERR_T_STRING[res]);
    }

    res = client_loop();
    if (OK != res)
    {
        printf("Unable to continue maun loop: %s\n", PROTO_ERR_T_STRING[res]);
    }

    return 0;
}
