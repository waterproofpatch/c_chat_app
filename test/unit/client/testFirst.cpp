#include "CppUTest/TestHarness.h"

// FUT
#include "protoBroadcastMessage.hpp"

// system includes
#include <stdio.h>

// clang-format off
TEST_GROUP(FirstTestGroup)
{
    TEST_SETUP()
    {
        printf("Setting up...");
    }
    TEST_TEARDOWN()
    {
        printf("Tearing down...");
    }
};
// clang-format on

TEST(FirstTestGroup, FirstTest)
{
    proto_err_t res = protoBroadcastMessage(NULL, NULL, 0, NULL, 0);
    LONGS_EQUAL(res, OK);
    /* FAIL("Fail me!"); */
}
