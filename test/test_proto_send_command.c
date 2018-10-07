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
 * @brief Test sending an arbitrary command
 * 
 */
void test_proto_send_command()
{
    char test_payload[10];
    TEST_ASSERT_TRUE(OK == proto_send_command(1, 2, test_payload, 10));
}