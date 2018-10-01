#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "protocol.h"
#include "error_codes.h"
#include "user.h"
#include "list.h"

// 'private' functions
/**
 * @brief handle sending a command
 * @param sock_fd: the socket to send the command to
 * @param cmd_type: the command_type_t value
 * @param payload: the optional payload for this command
 * @param payload_length: the length of the payload, if any (0 for NULL
 * payloads)
 */
static proto_err_t proto_send_command(int            sock_fd,
                                      command_type_t cmd_type,
                                      char *         payload,
                                      size_t         payload_length);

static proto_err_t proto_send_command(int            sock_fd,
                                      command_type_t cmd_type,
                                      char *         payload,
                                      size_t         payload_length)
{
    command_t *cmd = malloc(sizeof(command_t) + payload_length);
    if (!cmd)
    {
        return ERR_NO_MEM;
    }

    // zero out command
    memset(cmd, 0, sizeof(command_t) + payload_length);
    if (payload)
    {
        memcpy(cmd->payload, payload, payload_length);
    }

    // copy command attributes
    cmd->command_type   = cmd_type;
    cmd->payload_length = payload_length;

    // send the command to the remote machine
    if (write(sock_fd, cmd, sizeof(command_t) + payload_length) < 0)
    {
        return ERR_NETWORK_FAILURE;
    }
    return OK;
}

proto_err_t proto_read_command(int sock_fd, command_t **cmd_out)
{
    // command struct to use when figuring out payload length
    command_t cmd_hdr = {0};

    // command to return
    command_t *result_command = NULL;

    *cmd_out = NULL;

    int bytes_read = read(sock_fd, &cmd_hdr, sizeof(command_t));
    if (bytes_read == 0)
    {
        printf("No more data. Remote host closed?\n");
        return ERR_REMOTE_HOST_CLOSED;
    }
    if (bytes_read < 0)
    {
        printf("Failed reading cmd_hdr\n");
        return ERR_NETWORK_FAILURE;
    }
    if (bytes_read < sizeof(command_t))
    {
        printf("Not enough data for a command\n");
        return ERR_INVALID_COMMAND;
    }
    printf("Read command with payload length %d\n", cmd_hdr.payload_length);
    if (cmd_hdr.payload_length > CMD_MAX_PAYLOAD_LENGTH - 1)
    {
        printf(
            "Remote host wants to send too much payload data. Not reading.\n");
        return ERR_PAYLOAD_TOO_LARGE;   // TODO maybe kick?
    }
    // now we know the size of the full resulting command, copy in header and
    // read payload
    result_command = malloc(sizeof(command_t) + cmd_hdr.payload_length);
    if (!result_command)
    {
        printf("Unable to allocate command!\n");
        return ERR_NO_MEM;
    }

    memset(result_command, 0, sizeof(command_t) + cmd_hdr.payload_length);
    // copy in the command header information
    memcpy(result_command, &cmd_hdr, sizeof(command_t));
    if (cmd_hdr.payload_length > 0)
    {
        // read payload information
        if (read(sock_fd, result_command->payload, cmd_hdr.payload_length) !=
            cmd_hdr.payload_length)
        {
            printf("Unable to read %d bytes from remote host\n",
                   cmd_hdr.payload_length);
            free(result_command);
            return ERR_NETWORK_FAILURE;
        }
    }
    *cmd_out = result_command;
    return OK;
}

void proto_print_command(command_t *command)
{
    printf("command->command_type: %s\n",
           COMMAND_TYPE_T_STRING[command->command_type]);
    printf("command->payload_length: %d\n", command->payload_length);
    int i;
    for (i = 0; i < command->payload_length; i++)
    {
        printf("[0x%02x] ", command->payload[i]);
    }
    printf("\n");
}

proto_err_t proto_disconnect_client(int sock_fd, char *reason)
{
    proto_err_t status = OK;
    status = proto_send_command(sock_fd, CMD_REQUEST_DISCONNECT, reason,
                                strlen(reason));
    close(sock_fd);
    return status;
}

proto_err_t proto_disconnect_from_server(int sock_fd, char *reason)
{
    proto_err_t status = OK;
    status = proto_send_command(sock_fd, CMD_REQUEST_DISCONNECT, reason,
                                strlen(reason));
    close(sock_fd);
    return status;
}

proto_err_t proto_read_client_name(int sock_fd, char **name_out)
{
    proto_err_t status   = OK;
    command_t * cmd_name = NULL;
    *name_out            = NULL;

    status = proto_read_command(sock_fd, &cmd_name);
    if (status != OK)
    {
        printf("Unable to read client name.\n");
        goto done;
    }
    if (cmd_name->command_type != CMD_RESPONSE_NAME)
    {
        printf("Command is not CMD_RESPONSE_NAME\n");
        status = ERR_INVALID_COMMAND;
        goto done;
    }
    if (cmd_name->payload_length < 1 ||
        cmd_name->payload_length > MAX_USER_NAME_LENGTH)
    {
        printf("Username supplied was invalid length!\n");
        status = ERR_INVALID_COMMAND;
        goto done;
    }

    char *name = malloc(cmd_name->payload_length + 1);
    if (!name)
    {
        printf("Unable to allocate a name buffer!\n");
        status = ERR_NO_MEM;
        goto done;
    }
    name[cmd_name->payload_length] = '\0';   // null terminate the name
    memcpy(name, cmd_name->payload, cmd_name->payload_length);
    *name_out = name;

done:
    // we're done with cmd_name, free it
    free(cmd_name);
    return status;
}

proto_err_t proto_request_userlist_from_server(int sock_fd)
{
    return proto_send_command(sock_fd, CMD_REQUEST_USERLIST_FROM_SERVER, NULL,
                              0);
}
proto_err_t proto_request_client_name(int sock_fd)
{
    return proto_send_command(sock_fd, CMD_REQUEST_NAME, NULL, 0);
}

proto_err_t proto_send_client_name(int sock_fd, char *name, size_t name_length)
{
    return proto_send_command(sock_fd, CMD_RESPONSE_NAME, name, name_length);
}

proto_err_t proto_send_user_list(int sock_fd, list_t *user_list)
{
    // TODO protect user list with semaphore, always (delegate through accessor)
    int          num_users = list_count(user_list);
    name_list_t *name_list = malloc(sizeof(name_list_t));
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
        strncpy(name_list->usernames[i], active_user->name, MAX_USER_NAME_LENGTH);
    }
    return proto_send_command(
        sock_fd, CMD_USER_LIST, (char *)name_list,
        sizeof(name_list_t) + (num_users * MAX_USER_NAME_LENGTH));
}