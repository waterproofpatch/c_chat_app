
#ifndef _TYPES_H_
#define _TYPES_H_

#include "constants.hpp"
#include "user.hpp"
#include "list.hpp"

typedef enum _command_type_t
{
    FOREACH_COMMAND_TYPE(GENERATE_ENUM)
} command_type_t;

__attribute__((unused)) static const char *COMMAND_TYPE_T_STRING[] = {
    FOREACH_COMMAND_TYPE(GENERATE_STRING)};

typedef struct _command_t
{
    command_type_t command_type;
    int            payload_length;
    char           payload[0];
} command_t;

// hold a list of usernames for sending from server to client
typedef struct _name_list_t
{
    int  num_names;
    char usernames[MAX_CLIENTS][MAX_USER_NAME_LENGTH];
} name_list_t;

typedef struct _broadcast_message_t
{
    char   name[MAX_USER_NAME_LENGTH];
    size_t message_length;
    char   message[0];
} broadcast_message_t;

#endif