#include <stdio.h>  /* fgets */
#include <string.h> /* strlen */

#include "clientGetLineFromUser.h"
#include "clientPrintPrompt.h"

/**
 * @brief shameless SO copy pasta to read a line from stdin
 *
 * @param buff buffer to fill with user data
 * @param buff_length length of the buffer
 * @return int 1 on success, -1 on failure
 */
int clientGetLineFromUser(char *buff, size_t buff_length)
{
    int ch, extra;

    // get line with buffer overrun protection.
    clientPrintPrompt();
    if (fgets(buff, buff_length, stdin) == NULL)
    {
        return -1;
    }

    // if it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff) - 1] != '\n')
    {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
        {
            extra = 1;
        }
        return (extra == 1) ? -1 : 1;
    }

    // otherwise remove newline and give string back to caller.
    buff[strlen(buff) - 1] = '\0';
    return 1;
}