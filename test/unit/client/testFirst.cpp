#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

// FUT
#include "protoBroadcastMessage.hpp"

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

ssize_t wrappers_write(user_t *user, void *buf, size_t nbyte)
{
    mock().actualCall("wrappers_write");
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

TEST(FirstTestGroup, FirstTest)
{
    mock().expectOneCall("wrappers_malloc");
    mock().expectOneCall("wrappers_free");
    mock().expectOneCall("wrappers_write");
    mock().expectNCalls(2, "wrappers_memcpy");
    mock().expectNCalls(3, "wrappers_memset");

    proto_err_t res = protoBroadcastMessage(&gUser, "name", 4, "message", 7);

    mock().checkExpectations();

    LONGS_EQUAL(res, OK);
    /* FAIL("Fail me!"); */
}
