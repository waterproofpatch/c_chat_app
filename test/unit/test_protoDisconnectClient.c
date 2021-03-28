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
#include "mock_protoSendCommand.h"

/* globals */
static const char *gReason = "some_reason";
static user_t      gUser;

void tearDown()
{
}

/**
 * @brief test disconnecting a client
 *
 */
void test_protoDisconnectClient()
{

    protoSendCommand_ExpectAndReturn(
        &gUser, CMD_SHARED_REQUEST_DISCONNECT, gReason, strlen(gReason), 0);

    TEST_ASSERT_EQUAL(OK, protoDisconnectClient(&gUser, gReason));
}
