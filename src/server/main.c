/**
 * @file main.c
 * @author waterproofpatch
 * @brief main server program
 * @version 0.1
 * @date 2018-10-14
 *
 * @copyright Copyright (c) 2018
 *
 */
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <err.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#include "errorCodes.h"
#include "server.h"
#include "serverCreate.h"
#include "serverHandleConnections.h"

int main(int argc, char *argv[])
{
    // we'll listen on a high port to avoid having to sudo
    proto_err_t    status  = OK;
    unsigned short port_no = 5001;

    // create the server
    DBG_INFO("server running on port %d\n", port_no);
    status = serverCreate(port_no);
    if (status != OK)
    {
        DBG_INFO("Unable to create server: %s\n", PROTO_ERR_T_STRING[status]);
        return 1;
    }

    while (1)
    {
        serverHandleConnections();
    }
    return 0;
}
