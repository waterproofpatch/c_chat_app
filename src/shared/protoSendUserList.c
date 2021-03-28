
#include <string.h>

#include "protoSendUserList.h"
#include "protoSendCommand.h"
#include "wrappers.h"
#include "protocol.h"
#include "error_codes.h"
#include "user.h"
#include "list.h"

proto_err_t protoSendUserList(user_t *user, list_t *user_list)
{
    // TODO protect user list with semaphore, always (delegate through accessor)
    int          num_users = list_count(user_list);
    name_list_t *name_list = wrappers_malloc(sizeof(name_list_t));
    if (!name_list)
    {
        return ERR_NO_MEM;
    }
    name_list->num_names = num_users;

    for (int i = 0; i < name_list->num_names; i++)
    {
        user_t *active_user = list_get_at_index(user_list, i);
        if (!active_user)
        {
            return ERR_GENERAL;
        }
        strncpy(
            name_list->usernames[i], active_user->name, MAX_USER_NAME_LENGTH);
    }
    proto_err_t res = protoSendCommand(
        user,
        CMD_SERVER_USER_LIST,
        (char *)name_list,
        sizeof(name_list_t) + (num_users * MAX_USER_NAME_LENGTH));
    wrappers_free(name_list);
    return res;
}