
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define MAX_CLIENTS 10   // maximum number of clients we allow to be connected
#define MAX_MESSAGE_LENGTH 3000   // maximum length for a message
#define MAX_PAYLOAD_LENGTH \
    1000   // the maximum length allowed for any command payload
#define CMD_MAX_NAME_LENGTH 20   // max length for a command name string

// conditional static for test
#ifndef TEST
#define STATIC static
#else
#define STATIC
#endif

#define FOREACH_COMMAND_TYPE(RENDER)                \
    RENDER(CMD_SERVER_REQUEST_NAME)                 \
    RENDER(CMD_SHARED_REQUEST_DISCONNECT)           \
    RENDER(CMD_CLIENT_RESPONSE_NAME)                \
    RENDER(CMD_CLIENT_REQUEST_USERLIST_FROM_SERVER) \
    RENDER(CMD_SERVER_USER_LIST)                    \
    RENDER(CMD_CLIENT_BROADCAST_MESSAGE)            \
    RENDER(CMD_SERVER_BROADCAST_MESSAGE)            \
    RENDER(CMD_CANARY)                              \
    RENDER(CMD_PADDING = 0xffffffff)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#endif