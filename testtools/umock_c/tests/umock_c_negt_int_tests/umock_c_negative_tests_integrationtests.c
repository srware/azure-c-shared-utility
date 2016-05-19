// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"

#include "umock_c.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "test_dependency.h"
#include "umocktypes_charptr.h"

static TEST_MUTEX_HANDLE test_mutex;
static TEST_MUTEX_HANDLE global_mutex;

static void test_on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    (void)error_code;
}

int function_under_test_1_call(void)
{
    int result;
    
    if (function_1() != 0)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }

    return result;
}

int function_under_test_2_calls(void)
{
    int result;

    if (function_1() != 0)
    {
        result = __LINE__;
    }
    else
    {
        if (function_2() != 0)
        {
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }

    return result;
}

int function_under_test_1_call_dep_void_return(void)
{
    function_3_void_return();
    return 0;
}

BEGIN_TEST_SUITE(umock_c_negative_tests_integrationtests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(global_mutex);

    test_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_mutex);

    result = umock_c_init(test_on_umock_c_error);
    ASSERT_ARE_EQUAL(int, 0, result);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(test_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(global_mutex);
}

TEST_FUNCTION_INITIALIZE(test_function_init)
{
    int result = TEST_MUTEX_ACQUIRE(test_mutex);
    ASSERT_ARE_EQUAL(int, 0, result);

    result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, result);
}

TEST_FUNCTION_CLEANUP(test_function_cleanup)
{
    umock_c_negative_tests_deinit();
    umock_c_reset_all_calls();
    TEST_MUTEX_RELEASE(test_mutex);
}

/* Tests_SRS_UMOCK_C_LIB_01_167: [ umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path). ]*/
/* Tests_SRS_UMOCK_C_LIB_01_170: [ umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_173: [ umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_176: [ umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases. ]*/
TEST_FUNCTION(negative_tests_with_one_call)
{
    size_t i;
    STRICT_EXPECTED_CALL(function_1())
        .SetReturn(0).SetFailReturn(1);
    umock_c_negative_tests_snapshot();

    for (i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        // arrange
        char temp_str[128];
        int result;
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        // act
        result = function_under_test_1_call();

        // assert
        sprintf(temp_str, "On failed call %zu", i + 1);
        ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
    }
}

/* Tests_SRS_UMOCK_C_LIB_01_167: [ umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path). ]*/
/* Tests_SRS_UMOCK_C_LIB_01_170: [ umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_173: [ umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_176: [ umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases. ]*/
TEST_FUNCTION(negative_tests_with_2_calls)
{
    size_t i;
    STRICT_EXPECTED_CALL(function_1())
        .SetReturn(0).SetFailReturn(1);
    STRICT_EXPECTED_CALL(function_2())
        .SetReturn(0).SetFailReturn(1);
    umock_c_negative_tests_snapshot();

    for (i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        // arrange
        char temp_str[128];
        int result;
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        // act
        result = function_under_test_2_calls();

        // assert
        sprintf(temp_str, "On failed call %zu", i + 1);
        ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
    }
}

/* Tests_SRS_UMOCK_C_LIB_01_167: [ umock_c_negative_tests_snapshot shall take a snapshot of the current setup of expected calls (a.k.a happy path). ]*/
/* Tests_SRS_UMOCK_C_LIB_01_170: [ umock_c_negative_tests_reset shall bring umock_c expected and actual calls to the state recorded when umock_c_negative_tests_snapshot was called. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_173: [ umock_c_negative_tests_fail_call shall instruct the negative tests module to fail a specific call. ]*/
/* Tests_SRS_UMOCK_C_LIB_01_176: [ umock_c_negative_tests_call_count shall provide the number of expected calls, so that the test code can iterate through all negative cases. ]*/
TEST_FUNCTION(negative_tests_with_1_call_with_void_return_dependency)
{
    size_t i;
    STRICT_EXPECTED_CALL(function_3_void_return());
    umock_c_negative_tests_snapshot();

    for (i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        // arrange
        char temp_str[128];
        int result;
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        // act
        result = function_under_test_1_call_dep_void_return();

        // assert
        sprintf(temp_str, "On failed call %zu", i + 1);
        ASSERT_ARE_EQUAL_WITH_MSG(int, 0, result, temp_str);
    }
}

END_TEST_SUITE(umock_c_negative_tests_integrationtests)