#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

// FUT
#include "protoBroadcastMessage.hpp"

// project libraries
#include "types.hpp"   // command_type_t

// system includes
#include <stdio.h>
#include <string.h>

// installed library includes
#include <openssl/ssl.h>   // SSL

// globals
static user_t gUser;

// clang-format off
TEST_GROUP(FirstTestGroup)
{
    TEST_SETUP()
    {
        printf("Setting up...");
        gUser.ssl = (SSL*)0xdeadbeef;
        gUser.client_socket_fd = 2;
        strcpy(gUser.name, "username");
    }
    TEST_TEARDOWN()
    {
        printf("Tearing down...");
        mock().clear();
    }
};
// clang-format on

void *wrappers_malloc(size_t size)
{
    mock().actualCall("wrappers_malloc");
    return malloc(size);
}

void wrappers_free(void *p)
{
    mock().actualCall("wrappers_free");
    free(p);
}

void *wrappers_memcpy(void *dst, void *src, size_t n)
{
    mock().actualCall("wrappers_memcpy");
    return NULL;
}

void *wrappers_memset(void *b, int c, size_t len)
{
    mock().actualCall("wrappers_memset");
    return NULL;
}
proto_err_t protoSendCommand(user_t *       user,
                             command_type_t cmd_type,
                             const char *   payload,
                             size_t         payload_length)
{
    mock().actualCall("protoSendCommand");
    return OK;
}

TEST(FirstTestGroup, FirstTest)
{
    mock().expectOneCall("wrappers_malloc");
    mock().expectOneCall("wrappers_free");
    mock().expectNCalls(2, "wrappers_memcpy");
    mock().expectNCalls(3, "wrappers_memset");
    mock().expectOneCall("protoSendCommand");

    proto_err_t res = protoBroadcastMessage(&gUser, "name", 4, "message", 7);

    mock().checkExpectations();

    LONGS_EQUAL(res, OK);
    /* FAIL("Fail me!"); */
}
