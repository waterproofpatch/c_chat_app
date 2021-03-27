/* test framework imports */
#include "unity.h"
#include "cmock.h"

/* system includes for test code */
#include <stdio.h>
#include <string.h>

/* FUT */
#include "protoDisconnectClient.h"

/* project includes */
#include "error_codes.h"
#include "protocol.h"

/* mocks */
#include "mock_list.h"
#include "mock_wrappers.h"
#include "mock_protoSendCommand.h"

/* globals */
static char *      gPtr;
static const char *gReason = "some_reason";
static int         gSockFd = 2;

void setUp()
{
    gPtr = malloc(10);
}

void tearDown()
{
}

void *malloc_callback(size_t size, int num_calls)
{
    return gPtr;
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
void test_protoDisconnectClient()
{

    protoSendCommand_ExpectAndReturn(
        gSockFd, CMD_SHARED_REQUEST_DISCONNECT, gReason, strlen(gReason), 0);

    TEST_ASSERT_EQUAL(OK, protoDisconnectClient(2, gReason));
}
