/* test framework imports */
#include "unity.h"
#include "cmock.h"

/* system includes for test code */
#include <string.h>

/* FUT */
#include "protoDisconnectFromServer.h"

/* project includes */
#include "error_codes.h"
#include "protocol.h"

/* mocks */
#include "mock_protoSendCommand.h"

/* globals */
static const char *gReason = "some_reason";
static int         gSockFd = 2;

void setUp()
{
}

void tearDown()
{
}

/**
 * @brief test disconnecting a client
 *
 */
void test_protoDisconnectFromServer()
{
    protoSendCommand_ExpectAndReturn(
        gSockFd, CMD_SHARED_REQUEST_DISCONNECT, gReason, strlen(gReason), OK);
    TEST_ASSERT_EQUAL(OK, protoDisconnectFromServer(gSockFd, gReason));
}
