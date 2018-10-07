#include "build/temp/_test_something.c"
#include "protocol.h"
#include "mock_list.h"
#include "error_codes.h"
#include "cmock.h"
#include "unity.h"












void setUp()

{

}

void tearDown()

{

}



void test_proto_send_command()

{

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((1)), (((void*)0)), (UNITY_UINT)(21), UNITY_DISPLAY_STYLE_INT);

    char test_payload[10];

    if ((OK == proto_send_command(1, 2, test_payload, 10))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(23)));};

}
