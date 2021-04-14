#include "clientPrintCommands.hpp"
#include "debug.hpp"

/**
 * @brief print the available commands
 */
void clientPrintCommands(void)
{
    DBG_INFO("Available commands:\n");
    DBG_INFO("/getusers - Get list of conneted users\n");
    DBG_INFO("/quit - Disconnect from server\n");
    DBG_INFO("@<username> - PM a user\n");
}
