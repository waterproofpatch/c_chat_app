/**
 * @file client.h
 * @author waterproofpatch
 * @brief forward declarations for the client public functions
 * @version 0.1
 * @date 2018-10-16
 *
 * @copyright Copyright (c) 2018
 *
 */
#ifndef _CLIENT_H
#define _CLIENT_H
#include "protocol.h"

/**
 * @brief loop through client to handle commands
 *
 * @return proto_err_t OK on clean exit, ERR_* otherwise
 */
proto_err_t client_loop();

/**
 * @brief connect to the server and create the socket
 *
 * @param hostname hostname to connect to
 * @param port_no port to connect to
 * @return proto_err_t OK on successful socket connection, ERR_* otherwise
 */
proto_err_t client_connect(char *hostname, unsigned short port_no);

/**
 * @brief receive function, called asynchronusly, blocks until a command is
 * received from the server.
 *
 * @param context passed from thread creation, will be the
 * short port_no.
 * @return void* NULL
 */
void *client_receive_function(void *context);

/**
 * @brief perform the protocol handshake with the server
 *
 * @param username username the client wants
 * @return proto_err_t OK on success, ERR_* others
 */
proto_err_t client_handshake(char *username);

#endif