#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdlib.h>
#include "user.h"
#include "error_codes.h"

#define CMD_MAX_PAYLOAD_LENGTH \
    256   // the maximum length allowed for any command payload
#define CMD_MAX_NAME_LENGTH 20   // max length for a command name string

#define FOREACH_COMMAND_TYPE(RENDER)         \
    RENDER(CMD_REQUEST_NAME)                 \
    RENDER(CMD_REQUEST_DISCONNECT)           \
    RENDER(CMD_RESPONSE_NAME)                \
    RENDER(CMD_REQUEST_USERLIST_FROM_SERVER) \
    RENDER(CMD_PADDING = 0xffffffff)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum _command_type_t
{
    FOREACH_COMMAND_TYPE(GENERATE_ENUM)
} command_type_t;

__attribute__((unused)) static const char *COMMAND_TYPE_T_STRING[] = {
    FOREACH_COMMAND_TYPE(GENERATE_STRING)
    };

typedef struct _command_t
{
    command_type_t command_type;
    int            payload_length;
    char           payload[0];
} command_t;

/**
 * @brief handle reading a command
 * @param sock_fd: the socket to read the command from
 * @param cmd_out: ptr to the allocated command received from remote host, if
 * return value is OK
 * @return: OK on success, one of proto_err_t otherwise
 */
proto_err_t proto_read_command(int sock_fd, command_t **cmd_out);
proto_err_t proto_disconnect_client(int sock_fd, char *reason);
proto_err_t proto_disconnect_from_server(int sock_fd, char *reason);
proto_err_t proto_request_client_name(int sock_fd);
proto_err_t proto_read_client_name(int sock_fd, char **name_out);
proto_err_t proto_send_client_name(int sock_fd, char *name, size_t name_length);
proto_err_t proto_request_userlist_from_server(int sock_fd);
void        proto_print_command(command_t *command);

#endif   // __PROTOCOL_H_