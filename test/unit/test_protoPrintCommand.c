/* test framework imports */
#include "unity.h"
#include "cmock.h"

/* system includes for test code */
#include <stdio.h>
#include <string.h>

/* FUT */
#include "protoPrintCommand.h"

/* project includes */
#include "error_codes.h"
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
void test_protoPrintCommand()
{
    command_t *cmd    = malloc(sizeof(command_t) + MAX_PAYLOAD_LENGTH - 1);
    cmd->command_type = CMD_CANARY - 1;
    memset(cmd->payload, 0, MAX_PAYLOAD_LENGTH - 1);
    cmd->payload_length = MAX_PAYLOAD_LENGTH - 1;
    protoPrintCommand(cmd);

    free(cmd);
}

/**
 * @brief Test printing a command that has an invalid command type
 *
 */
void test_protoPrintCommand_invalidCommandType()
{
    command_t *cmd      = malloc(sizeof(command_t) + MAX_PAYLOAD_LENGTH - 1);
    cmd->command_type   = CMD_CANARY + 1;
    cmd->payload_length = MAX_PAYLOAD_LENGTH - 1;
    memset(cmd->payload, 0, MAX_PAYLOAD_LENGTH - 1);
    protoPrintCommand(cmd);

    free(cmd);
}
