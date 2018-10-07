#include "unity.h"
#include "cmock.h"

#include <stdlib.h>
#include <string.h>

#include "error_codes.h"

// mocks
#include "mock_wrappers.h"
#include "mock_list.h"

// code code under test
#include "protocol.h"

void setUp()
{
}

void tearDown()
{
}

void* memcpy_callback(void* dst, void* src, size_t n, int num_calls)
{
    TEST_ASSERT_NOT_NULL(dst);
    TEST_ASSERT_NOT_NULL(src);
    if (num_calls == 0)
    {
        TEST_ASSERT_EQUAL(n, sizeof(command_t));
    }
    memcpy(dst, src, n);
    return dst;
}

ssize_t read_callback(int fd, void* buf_out, size_t size, int num_calls)
{
    TEST_ASSERT_NOT_NULL(buf_out);
    // first call, expecting to read a command_t header
    if (num_calls == 0)
    {
        TEST_ASSERT_EQUAL(2, fd);
        TEST_ASSERT_EQUAL(sizeof(command_t), size);
        command_t* ptr_command      = (command_t*)buf_out;
        ptr_command->command_type   = CMD_RESPONSE_NAME;
        ptr_command->payload_length = strlen("testname");
        return sizeof(command_t);
    }
    else if (num_calls == 1)
    {
        // reading the payload
        TEST_ASSERT_EQUAL(2, fd);
        TEST_ASSERT_EQUAL(strlen("testname"), size);
        memcpy(buf_out, "testname", strlen("testname"));
        return strlen("testname");
    }
    else
    {
        TEST_FAIL();
    }
    return 1;
}
/**
 * @brief read a legitimate command from a remote client
 *
 */
void test_proto_read_command()
{
    char*      ptr = malloc(sizeof(command_t) + strlen("testname"));
    command_t* cmd = NULL;

    wrappers_read_StubWithCallback(read_callback);
    wrappers_malloc_ExpectAndReturn(strlen("testname") + sizeof(command_t),
                                    ptr);
    wrappers_memset_ExpectAndReturn(
        ptr, 0, sizeof(command_t) + strlen("testname"), ptr);
    wrappers_memcpy_StubWithCallback(memcpy_callback);

    TEST_ASSERT_EQUAL(OK, proto_read_command(2, &cmd));
    TEST_ASSERT_NOT_NULL(cmd);
    TEST_ASSERT_EQUAL(strlen("testname"), cmd->payload_length);
    TEST_ASSERT_EQUAL_INT8_ARRAY("testname", cmd->payload, strlen("testname"));

    free(ptr);
}
