#ifndef __USER_H__
#define __USER_H__

#include <openssl/ssl.h>

#define MAX_USER_NAME_LENGTH 20

// represent a user object
typedef struct _user_t
{
    char name[MAX_USER_NAME_LENGTH];   // the name for the user
    int  client_socket_fd;             // the socket for the client
    SSL *ssl;                          // ssl context
} user_t;

/**
 * @brief a user comparator
 * @param context: ptr to the user object to compare
 * @param key: the name of the user to search for
 * @return 1 on equality, 0 otherwise
 */
char user_comparator(void *context, void *key);
#endif   // __USER_H__