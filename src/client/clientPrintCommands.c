#include <stdio.h> /* fgets */

#include "clientPrintCommands.h"

/**
 * @brief print the available commands
 */
void clientPrintCommands(void)
{
    printf("Available commands:\n");
    printf("/getusers - Get list of conneted users\n");
    printf("/quit - Disconnect from server\n");
    printf("@<username> - PM a user\n");
}
