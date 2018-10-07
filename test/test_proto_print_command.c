#include "unity.h"
#include "cmock.h"

#include <stdio.h>

#include "error_codes.h"

// mocks
#include "mock_list.h"

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
    char test_payload[10];
    proto_print_command((command_t*)test_payload);
}