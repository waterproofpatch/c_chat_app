#include <string.h>
#include "user.h"

char user_comparator(void *context, void *key)
{
    user_t *user = (user_t *)context;
    char *  name = (char *)key;
    return strcmp(name, user->name) == 0;
}
