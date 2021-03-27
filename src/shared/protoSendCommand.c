#include "protoSendCommand.h"
#include "protocol.h"
#include "error_codes.h"
#include "wrappers.h"

/**
 * @brief handle sending a command
 * @param sock_fd: the socket to send the command to
 * @param cmd_type: the command_type_t value
 * @param payload: the optional payload for this command
 * @param payload_length: the length of the payload, if any (0 for NULL
 * payloads)
 */
proto_err_t protoSendCommand(int            sock_fd,
                             command_type_t cmd_type,
                             const char *   payload,
                             size_t         payload_length)
{
    command_t *cmd = wrappers_malloc(sizeof(command_t) + payload_length);
    if (!cmd)
    {
        return ERR_NO_MEM;
    }

    // zero out command
    wrappers_memset(cmd, 0, sizeof(command_t) + payload_length);
    if (payload)
    {
        wrappers_memcpy(cmd->payload, payload, payload_length);
    }

    // copy command attributes
    cmd->command_type   = cmd_type;
    cmd->payload_length = payload_length;

    // send the command to the remote machine
    if (wrappers_write(sock_fd, cmd, sizeof(command_t) + payload_length) < 0)
    {
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}