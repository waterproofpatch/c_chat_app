
#include <stdio.h>

#include "protoPrintCommand.h"
#include "protoSendCommand.h"
#include "protocol.h" /* CMD */
#include "debug.h"

void protoPrintCommand(command_t *command)
{
    if (command->command_type >= CMD_CANARY)
    {
        DBG_ERROR("Command type %d is invalid.\n", command->command_type);
        return;
    }
    DBG_INFO("command->command_type: %s\n",
             COMMAND_TYPE_T_STRING[command->command_type]);
    DBG_INFO("command->payload_length: %d\n", command->payload_length);

    int i = 0;
    for (i = 0; i <= MAX_PAYLOAD_LENGTH && i < command->payload_length; i++)
    {
        DBG_ERROR("[0x%02x] ", command->payload[i]);
    }
    DBG_INFO("\n");
}