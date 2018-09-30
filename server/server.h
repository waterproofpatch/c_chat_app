#ifndef __SERVER_H_
#define __SERVER_H_

/**
 * @brief send a disconnect message to the client and then terminate the socket
 * @param sock_fd: the socket for the client
 */
void disconnect_client(int sock_fd);

char server_client_socket_fd_comparator(void *context, void *key);
void server_init_client_sockets();
void server_add_client_socket(int socket_fd);
void *server_handle_client_thread(void *context);

#endif // __SERVER_H_
