#include "unity.h"
#include "cmock.h"

#include <stdio.h>
#include <string.h>

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
    command_t *ptr = malloc(10 + sizeof(command_t));
    char       test_payload[10];

    wrappers_malloc_ExpectAndReturn(10 + sizeof(command_t), ptr);
    wrappers_memset_ExpectAndReturn(ptr, 0, sizeof(command_t) + 10, ptr);
    wrappers_memcpy_ExpectAndReturn(ptr->payload, &test_payload, 10,
                                    ptr->payload);
    wrappers_write_ExpectAndReturn(1, ptr, sizeof(command_t) + 10, 0);

    TEST_ASSERT_TRUE(OK == proto_send_command(1, 2, test_payload, 10));

    free(ptr);
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

/**
 * @brief Test sending an arbitrary command when writing to the socket fails
 *
 */
void test_proto_send_command_err_network_failure()
{
    command_t *ptr = malloc(10 + sizeof(command_t));
    char       test_payload[10];

    wrappers_malloc_ExpectAndReturn(10 + sizeof(command_t), ptr);
    wrappers_memset_ExpectAndReturn(ptr, 0, sizeof(command_t) + 10, ptr);
    wrappers_memcpy_ExpectAndReturn(ptr->payload, &test_payload, 10,
                                    ptr->payload);
    wrappers_write_ExpectAndReturn(1, ptr, sizeof(command_t) + 10, -1);

    TEST_ASSERT_TRUE(ERR_NETWORK_FAILURE ==
                     proto_send_command(1, 2, test_payload, 10));

    free(ptr);
}
