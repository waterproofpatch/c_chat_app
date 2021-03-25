
#include "serverClientSocketFdComparator.h"
#include "user.h"

/**
 * @brief print a user object via list_foreach
 *
 * @param user_opaque the user passed in via list_foreach.
 * @param context context provided to list_foreach.
 * @return 1 if @context user owns @key file descriptor.
 * @return 0 otherwise.
 */
char serverClientSocketFdComparator(void *context, void *key)
{
    user_t *user    = (user_t *)context;
    int     sock_fd = *(int *)key;
    return user->client_socket_fd == sock_fd;
}