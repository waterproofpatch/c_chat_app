#include "CppUTest/TestHarness.h"

// project includes
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
    protoBroadcastMessage(NULL, NULL, 0, NULL, 0);
    FAIL("Fail me!");
}

TEST(FirstTestGroup, SecondTest)
{
    FAIL("Fail me again!");
}