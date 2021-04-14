/**
 * @file protoSendCommand.c
 * @author waterproofpatch (waterproofpatch@gmail.com)
 * @brief send a command.
 * @version 0.1
 * @date 2021-03-27
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "protoSendCommand.h"
#include "types.h"
#include "errorCodes.h"
#include "wrappers.h"
#include "user.h"
#include "debug.h"

/**
 * @brief handle sending a command
 * @param sock_fd: the socket to send the command to
 * @param cmd_type: the command_type_t value
 * @param payload: the optional payload for this command
 * @param payload_length: the length of the payload, if any (0 for NULL
 * payloads)
 * @return ERR_GENERAL if an argument is bad.
 * @return ERR_NO_MEM if no memory could be allocated.
 * @return ERR_NETWORK_FAILURE if writing failed.
 */
proto_err_t protoSendCommand(user_t *       user,
                             command_type_t cmd_type,
                             const char *   payload,
                             size_t         payload_length)
{
    if (user == NULL)
    {
        DBG_ERROR("user is NULL!\n");
        return ERR_GENERAL;
    }
    if (user->ssl == NULL)
    {
        DBG_ERROR("user->ssl is NULL!\n");
        return ERR_GENERAL;
    }

    command_t *cmd =
        (command_t *)wrappers_malloc(sizeof(command_t) + payload_length);
    if (!cmd)
    {
        return ERR_NO_MEM;
    }

    // zero out command
    wrappers_memset(cmd, 0, sizeof(command_t) + payload_length);
    if (payload)
    {
        wrappers_memcpy(cmd->payload, (void *)payload, payload_length);
    }

    // copy command attributes
    cmd->command_type   = cmd_type;
    cmd->payload_length = payload_length;

    // send the command to the remote machine
    if (wrappers_write(user, cmd, sizeof(command_t) + payload_length) < 0)
    {
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}