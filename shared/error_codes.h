
#ifndef __ERROR_CODES_H_
#define __ERROR_CODES_H_

// preprocesor foo (from SO poster online) to generate C code
// that allows easy enum type to string mapping
#define FOREACH_PROTOCOL_ERR(ERROR) \
    ERROR(OK)                       \
    ERROR(ERR_NO_MEM)               \
    ERROR(ERR_INVALID_COMMAND)      \
    ERROR(ERR_PAYLOAD_TOO_LARGE)    \
    ERROR(ERR_NETWORK_FAILURE)      \
    ERROR(ERR_REMOTE_HOST_CLOSED)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum proto_err_t {
    FOREACH_PROTOCOL_ERR(GENERATE_ENUM)
} proto_err_t;

__attribute__((unused)) static const char *PROTO_ERR_T_STRING[] = {
    FOREACH_PROTOCOL_ERR(GENERATE_STRING)};

#endif // __ERROR_CODES_H_