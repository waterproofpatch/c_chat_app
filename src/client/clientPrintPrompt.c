
#include <stdio.h>

#include "clientPrintPrompt.h"

/**
 * @brief print a prompt for the user.
 */
void clientPrintPrompt(void)
{
    printf("%s", "> ");
    fflush(stdout);
}