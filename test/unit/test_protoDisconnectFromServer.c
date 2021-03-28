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
#include "user.h"

/* mocks */
#include "mock_protoSendCommand.h"

/* globals */
static const char *gReason = "some_reason";
static user_t      gUser;

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
        &gUser, CMD_SHARED_REQUEST_DISCONNECT, gReason, strlen(gReason), OK);
    TEST_ASSERT_EQUAL(OK, protoDisconnectFromServer(&gUser, gReason));
}
