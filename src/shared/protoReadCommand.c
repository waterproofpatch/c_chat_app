
#include <stdio.h>

#include "protoReadCommand.h"
#include "protocol.h"
#include "error_codes.h"
#include "wrappers.h"

proto_err_t protoReadCommand(int sock_fd, command_t **cmd_out)
{
    // command struct to use when figuring out payload length
    command_t cmd_hdr = {0};

    // command to return
    command_t *result_command = NULL;

    *cmd_out = NULL;

    int bytes_wrappers_read =
        wrappers_read(sock_fd, &cmd_hdr, sizeof(command_t));
    if (bytes_wrappers_read == 0)
    {
        printf("No more data. Remote host closed?\n");
        return ERR_REMOTE_HOST_CLOSED;
    }
    if (bytes_wrappers_read < 0)
    {
        printf("Failed wrappers_reading cmd_hdr\n");
        return ERR_NETWORK_FAILURE;
    }
    if (bytes_wrappers_read < sizeof(command_t))
    {
        printf("Not enough data for a command\n");
        return ERR_INVALID_COMMAND;
    }
    if (cmd_hdr.payload_length > MAX_PAYLOAD_LENGTH - 1)
    {
        printf(
            "Remote host wants to send too much payload data. Not "
            "wrappers_reading.\n");
        return ERR_PAYLOAD_TOO_LARGE;
    }
    // now we know the size of the full resulting command, copy in header and
    // wrappers_read payload
    result_command =
        wrappers_malloc(sizeof(command_t) + cmd_hdr.payload_length);
    if (!result_command)
    {
        printf("Unable to allocate command!\n");
        return ERR_NO_MEM;
    }

    wrappers_memset(
        result_command, 0, sizeof(command_t) + cmd_hdr.payload_length);
    // copy in the command header information
    wrappers_memcpy(result_command, &cmd_hdr, sizeof(command_t));
    if (cmd_hdr.payload_length > 0)
    {
        // wrappers_read payload information
        if (wrappers_read(sock_fd,
                          result_command->payload,
                          cmd_hdr.payload_length) != cmd_hdr.payload_length)
        {
            printf("Unable to wrappers_read %d bytes from remote host\n",
                   cmd_hdr.payload_length);
            wrappers_free(result_command);
            return ERR_NETWORK_FAILURE;
        }
    }
    *cmd_out = result_command;
    return OK;
}