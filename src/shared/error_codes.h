
#ifndef __ERROR_CODES_H_
#define __ERROR_CODES_H_

// Populate list of errors rendered according to
// supplied 'RENDER' macro function.
#define FOREACH_PROTOCOL_ERR(RENDER) \
    RENDER(OK)                       \
    RENDER(ERR_SSL)                  \
    RENDER(ERR_NO_MEM)               \
    RENDER(ERR_INVALID_COMMAND)      \
    RENDER(ERR_PAYLOAD_TOO_LARGE)    \
    RENDER(ERR_NETWORK_FAILURE)      \
    RENDER(ERR_REMOTE_HOST_CLOSED)   \
    RENDER(ERR_GENERAL)

// get enum type value
#define GENERATE_ENUM(ENUM) ENUM,

// get human readable string value
#define GENERATE_STRING(STRING) #STRING,

typedef enum proto_err_t
{
    FOREACH_PROTOCOL_ERR(GENERATE_ENUM)
} proto_err_t;

__attribute__((unused)) static const char *PROTO_ERR_T_STRING[] = {
    FOREACH_PROTOCOL_ERR(GENERATE_STRING)};

#endif   // __ERROR_CODES_H_
