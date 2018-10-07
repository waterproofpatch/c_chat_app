
#include <stdlib.h>
#include "wrappers.h"

void* wrappers_malloc(size_t size)
{
    return malloc(size);
}