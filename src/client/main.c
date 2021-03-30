#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "clientHandshake.h"
#include "clientLoop.h"
#include "clientConnect.h"
#include "errorCodes.h"
#include "debug.h"

int main(int argc, char **argv)
{
    proto_err_t res = OK;
    DBG_INFO("Starting client...\n");

    if (argc < 2)
    {
        DBG_INFO("Usage: ./client <username>\n");
        return -1;
    }
    unsigned short port_no    = 5001;
    char           hostname[] = "127.0.0.1";

    res = clientConnect(hostname, port_no);
    if (OK != res)
    {
        DBG_ERROR("Unable to connect: %s\n", PROTO_ERR_T_STRING[res]);
        return res;
    }

    // send name to server
    res = clientHandshake(argv[1]);
    if (OK != res)
    {
        DBG_ERROR("Unable to complete handshake: %s\n",
                  PROTO_ERR_T_STRING[res]);
        return res;
    }

    res = clientLoop();
    if (OK != res)
    {
        DBG_ERROR("Unable to continue maun loop: %s\n",
                  PROTO_ERR_T_STRING[res]);
        return res;
    }

    return 0;
}
