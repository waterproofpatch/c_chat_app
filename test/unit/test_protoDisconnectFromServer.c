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

static char *g_ptr;

void setUp()
{
    g_ptr = malloc(10);
}

void tearDown()
{
}

void *malloc_callback(size_t size, int num_calls)
{
    return g_ptr;
}

void *memcpy_callback(void *dst, void *src, size_t n, int num_calls)
{
    return memcpy(dst, src, n);
}

void *memset_callback(void *dst, int c, size_t n, int num_calls)
{
    return memset(dst, c, n);
}

/**
 * @brief test disconnecting a client
 *
 */
void test_proto_disconnect_from_server()
{
    wrappers_write_ExpectAndReturn(2,
                                   g_ptr,
                                   sizeof(command_t) + strlen("some_reason"),
                                   sizeof(command_t) + strlen("some_reason"));
    wrappers_malloc_StubWithCallback(malloc_callback);
    wrappers_memset_StubWithCallback(memset_callback);
    wrappers_memcpy_StubWithCallback(memcpy_callback);
    TEST_ASSERT_EQUAL(OK, protoDisconnectFromServer(2, "some_reason"));
}
