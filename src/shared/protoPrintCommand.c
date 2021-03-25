
#include <stdio.h>

#include "protoPrintCommand.h"
#include "protoSendCommand.h"
#include "protocol.h" /* CMD */

void protoPrintCommand(command_t *command)
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