#include "CppUTest/TestHarness.h"
// #include "ClassName.h"

class A
{
   public:
   private:
};

TEST_GROUP(A)
{
    A *className;

    void setup()
    {
        className = new A();
    }
    void teardown()
    {
        delete className;
    }
};

TEST(A, Create)
{
    CHECK(0 != className);
    CHECK(true);
    CHECK_EQUAL(1, 1);
    LONGS_EQUAL(1, 1);
    DOUBLES_EQUAL(1.000, 1.001, .01);
    STRCMP_EQUAL("hello", "hello");
    FAIL("The prior tests pass, but this one doesn't");
}