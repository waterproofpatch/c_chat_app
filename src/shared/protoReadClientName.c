#include "protoReadClientName.h"
#include "protoReadCommand.h"
#include "errorCodes.h" /* proto_err_t */
#include "types.h"      /* CMD */
#include "wrappers.h"
#include "user.h"
#include "debug.h"

proto_err_t protoReadClientName(user_t *user, char **name_out)
{
    proto_err_t status   = OK;
    command_t * cmd_name = NULL;
    *name_out            = NULL;
    char *name           = NULL;

    status = protoReadCommand(user, &cmd_name);
    if (status != OK)
    {
        DBG_ERROR("Unable to wrappers_read client name.\n");
        goto done;
    }
    if (cmd_name->command_type != CMD_CLIENT_RESPONSE_NAME)
    {
        DBG_ERROR("Command is not response to name request\n");
        status = ERR_INVALID_COMMAND;
        goto done;
    }
    if (cmd_name->payload_length < 1 ||
        cmd_name->payload_length > MAX_USER_NAME_LENGTH)
    {
        DBG_ERROR("Username supplied was invalid length!\n");
        status = ERR_INVALID_COMMAND;
        goto done;
    }
    name = (char *)wrappers_malloc(cmd_name->payload_length + 1);
    if (!name)
    {
        DBG_ERROR("Unable to allocate a name buffer!\n");
        status = ERR_NO_MEM;
        goto done;
    }

    name[cmd_name->payload_length] = '\0';   // null terminate the name
    wrappers_memcpy(name, cmd_name->payload, cmd_name->payload_length);
    *name_out = name;

done:
    // we're done with cmd_name, wrappers_free it
    if (cmd_name)
    {
        wrappers_free(cmd_name);
        cmd_name = NULL;
        if (status != OK && name)
        {
            wrappers_free(name);
            name = NULL;
        }
    }
    return status;
}