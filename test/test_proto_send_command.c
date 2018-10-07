#include "unity.h"
#include "cmock.h"

#include <stdio.h>

#include "error_codes.h"

// mocks
#include "mock_list.h"
#include "mock_wrappers.h"

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
    wrappers_malloc_ExpectAndReturn(10 + sizeof(command_t), malloc(10+sizeof(command_t)));

    char test_payload[10];
    TEST_ASSERT_TRUE(OK == proto_send_command(1, 2, test_payload, 10));
}

/**
 * @brief Test sending arbitrary command but with no memory
 * 
 */
void test_proto_send_command_err_no_mem()
{
    wrappers_malloc_ExpectAndReturn(10 + sizeof(command_t), NULL);

    char test_payload[10];
    TEST_ASSERT_TRUE(ERR_NO_MEM == proto_send_command(1, 2, test_payload, 10));
}