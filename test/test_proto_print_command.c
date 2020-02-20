#include "unity.h"
#include "cmock.h"

#include <stdio.h>
#include <string.h>

#include "error_codes.h"

// mocks
#include "mock_list.h"
#include "mock_wrappers.h"

// protocol
#include "protocol.h"

void setUp()
{
}

void tearDown()
{
}

/**
 * @brief Test printing a command
 *
 */
void test_proto_print_command()
{
    command_t *cmd    = malloc(sizeof(command_t) + MAX_PAYLOAD_LENGTH - 1);
    cmd->command_type = CMD_CANARY - 1;
    memset(cmd->payload, 0, MAX_PAYLOAD_LENGTH - 1);
    cmd->payload_length = MAX_PAYLOAD_LENGTH - 1;
    proto_print_command(cmd);

    free(cmd);
}

/**
 * @brief Test printing a command that has an invalid command type
 *
 */
void test_proto_print_command_invalid_command_type()
{
    command_t *cmd      = malloc(sizeof(command_t) + MAX_PAYLOAD_LENGTH - 1);
    cmd->command_type   = CMD_CANARY + 1;
    cmd->payload_length = MAX_PAYLOAD_LENGTH - 1;
    memset(cmd->payload, 0, MAX_PAYLOAD_LENGTH - 1);
    proto_print_command(cmd);

    free(cmd);
}
