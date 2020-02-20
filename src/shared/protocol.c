#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "protocol.h"
#include "wrappers.h"
#include "error_codes.h"
#include "user.h"
#include "list.h"

/**
 * @brief handle sending a command
 * @param sock_fd: the socket to send the command to
 * @param cmd_type: the command_type_t value
 * @param payload: the optional payload for this command
 * @param payload_length: the length of the payload, if any (0 for NULL
 * payloads)
 */
STATIC proto_err_t proto_send_command(int            sock_fd,
                                      command_type_t cmd_type,
                                      char *         payload,
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

proto_err_t proto_read_command(int sock_fd, command_t **cmd_out)
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

    wrappers_memset(result_command, 0,
                    sizeof(command_t) + cmd_hdr.payload_length);
    // copy in the command header information
    wrappers_memcpy(result_command, &cmd_hdr, sizeof(command_t));
    if (cmd_hdr.payload_length > 0)
    {
        // wrappers_read payload information
        if (wrappers_read(sock_fd, result_command->payload,
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

void proto_print_command(command_t *command)
{
    if (command->command_type >= CMD_CANARY)
    {
        printf("Command type %d is invalid.\n", command->command_type);
        return;
    }
    printf("command->command_type: %s\n",
           COMMAND_TYPE_T_STRING[command->command_type]);
    printf("command->payload_length: %d\n", command->payload_length);
    int i;
    for (i = 0; i <= MAX_PAYLOAD_LENGTH && i < command->payload_length; i++)
    {
        printf("[0x%02x] ", command->payload[i]);
    }
    printf("\n");
}

proto_err_t proto_disconnect_client(int sock_fd, char *reason)
{
    proto_err_t status = OK;
    status = proto_send_command(sock_fd, CMD_SHARED_REQUEST_DISCONNECT, reason,
                                strlen(reason));
    return status;
}

proto_err_t proto_disconnect_from_server(int sock_fd, char *reason)
{
    proto_err_t status = OK;
    status = proto_send_command(sock_fd, CMD_SHARED_REQUEST_DISCONNECT, reason,
                                strlen(reason));
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
        printf("Unable to wrappers_read client name.\n");
        goto done;
    }
    if (cmd_name->command_type != CMD_CLIENT_RESPONSE_NAME)
    {
        printf("Command is not response to name request\n");
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

    char *name = wrappers_malloc(cmd_name->payload_length + 1);
    if (!name)
    {
        printf("Unable to allocate a name buffer!\n");
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
    }
    return status;
}

proto_err_t proto_send_global_message(int sock_fd, char *buffer)
{
    return proto_send_command(sock_fd, CMD_CLIENT_BROADCAST_MESSAGE, buffer,
                              strlen(buffer));
}

proto_err_t proto_broadcast_message(int    sock_fd,
                                    char * name,
                                    size_t name_length,
                                    char * message,
                                    size_t message_length)
{
    if (message_length > MAX_MESSAGE_LENGTH)
    {
        return ERR_PAYLOAD_TOO_LARGE;
    }
    broadcast_message_t *broadcast_message =
        wrappers_malloc(sizeof(broadcast_message_t) + message_length + 1);

    if (!broadcast_message)
    {
        return ERR_NO_MEM;
    }
    wrappers_memset(broadcast_message, 0,
                    sizeof(broadcast_message_t) + message_length + 1);
    wrappers_memset(broadcast_message->name, '\0', MAX_USER_NAME_LENGTH);
    wrappers_memset(broadcast_message->message, '\0', message_length + 1);
    wrappers_memcpy(broadcast_message->name, name, name_length);
    wrappers_memcpy(broadcast_message->message, message, message_length);
    broadcast_message->message_length = message_length;
    proto_err_t res                   = proto_send_command(
        sock_fd, CMD_SERVER_BROADCAST_MESSAGE, (char *)broadcast_message,
        sizeof(broadcast_message_t) + message_length + 1);
    wrappers_free(broadcast_message);
    return res;
}

proto_err_t proto_request_userlist_from_server(int sock_fd)
{
    return proto_send_command(sock_fd, CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER,
                              NULL, 0);
}
proto_err_t proto_request_client_name(int sock_fd)
{
    return proto_send_command(sock_fd, CMD_SERVER_REQUEST_NAME, NULL, 0);
}

proto_err_t proto_send_client_name(int sock_fd, char *name, size_t name_length)
{
    return proto_send_command(sock_fd, CMD_CLIENT_RESPONSE_NAME, name,
                              name_length);
}

proto_err_t proto_send_user_list(int sock_fd, list_t *user_list)
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
        strncpy(name_list->usernames[i], active_user->name,
                MAX_USER_NAME_LENGTH);
    }
    proto_err_t res = proto_send_command(
        sock_fd, CMD_SERVER_USER_LIST, (char *)name_list,
        sizeof(name_list_t) + (num_users * MAX_USER_NAME_LENGTH));
    wrappers_free(name_list);
    return res;
}
