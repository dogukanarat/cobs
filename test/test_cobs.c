/* test_cobs.c - Unit tests for cobs */

/* All Rights Reserved */

/* Includes */

#include "cobs/cobs.h"
#include "unity/unity.h"
#include <limits.h>
#include <string.h>

/* Test Setup and Teardown */

void setUp(void)
{
    /* This is run before EACH test */
}

void tearDown(void)
{
    /* This is run after EACH test */
}

/* Test Cases for cobsGetVersion */

void test_get_version_should_return_version_string(void)
{
    const char *version = cobs_get_version();
    TEST_ASSERT_NOT_NULL(version);
    TEST_ASSERT_EQUAL_STRING("0.0.1", version);
}

/* Test Cases for cobsAdd */

void test_add_should_return_sum_when_adding_positive_numbers(void)
{
    int32_t result = cobs_add(5, 3);
    TEST_ASSERT_EQUAL_INT32(8, result);
}

void test_add_should_return_sum_when_adding_negative_numbers(void)
{
    int32_t result = cobs_add(-5, -3);
    TEST_ASSERT_EQUAL_INT32(-8, result);
}

void test_add_should_return_sum_when_adding_mixed_numbers(void)
{
    int32_t result = cobs_add(10, -5);
    TEST_ASSERT_EQUAL_INT32(5, result);
}

void test_add_should_return_zero_when_adding_zeros(void)
{
    int32_t result = cobs_add(0, 0);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_add_should_saturate_on_overflow(void)
{
    int32_t result = cobs_add(INT32_MAX, 1);
    TEST_ASSERT_EQUAL_INT32(INT32_MAX, result);
}

void test_add_should_saturate_on_underflow(void)
{
    int32_t result = cobs_add(INT32_MIN, -1);
    TEST_ASSERT_EQUAL_INT32(INT32_MIN, result);
}

/* Test Cases for cobsMultiply */

void test_multiply_should_return_success_when_multiplying_positive_numbers(void)
{
    int32_t result;
    cobs_status_t status = cobs_multiply(5, 3, &result);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(15, result);
}

void test_multiply_should_return_success_when_multiplying_by_zero(void)
{
    int32_t result;
    cobs_status_t status = cobs_multiply(5, 0, &result);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_multiply_should_return_error_null_when_result_pointer_is_null(void)
{
    cobs_status_t status = cobs_multiply(5, 3, NULL);
    TEST_ASSERT_EQUAL(COBS_ERROR_NULL, status);
}

void test_multiply_should_return_error_invalid_on_overflow(void)
{
    int32_t result = 0;
    cobs_status_t status = cobs_multiply(INT32_MAX, 2, &result);
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, status);
}

void test_multiply_should_return_error_invalid_on_underflow(void)
{
    int32_t result = 0;
    cobs_status_t status = cobs_multiply(INT32_MIN, -1, &result);
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, status);
}

/* Test Cases for cobsFoo */

void test_foo_should_return_success_when_processing_valid_input(void)
{
    char output[100];
    cobs_status_t status = cobs_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(COBS_SUCCESS, status);
    TEST_ASSERT_EQUAL_STRING("Processed: test", output);
}

void test_foo_should_return_error_null_when_input_is_null(void)
{
    char output[100];
    cobs_status_t status = cobs_foo(NULL, output, sizeof(output));
    TEST_ASSERT_EQUAL(COBS_ERROR_NULL, status);
}

void test_foo_should_return_error_null_when_output_is_null(void)
{
    cobs_status_t status = cobs_foo("test", NULL, 100);
    TEST_ASSERT_EQUAL(COBS_ERROR_NULL, status);
}

void test_foo_should_return_error_invalid_when_output_size_is_zero(void)
{
    char output[100];
    cobs_status_t status = cobs_foo("test", output, 0);
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, status);
}

void test_foo_should_return_error_invalid_when_buffer_too_small(void)
{
    char output[5];
    cobs_status_t status = cobs_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, status);
}

void test_foo_should_succeed_when_buffer_exact_size(void)
{
    char output[16];
    cobs_status_t status = cobs_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(COBS_SUCCESS, status);
    TEST_ASSERT_EQUAL_STRING("Processed: test", output);
}

void test_foo_should_return_error_invalid_when_buffer_one_byte_short(void)
{
    char output[15];
    cobs_status_t status = cobs_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, status);
}

/* Test Cases for cobsBar */

void test_bar_should_return_true_when_value_is_in_range(void)
{
    TEST_ASSERT_TRUE(cobs_bar(50));
    TEST_ASSERT_TRUE(cobs_bar(0));
    TEST_ASSERT_TRUE(cobs_bar(100));
}

void test_bar_should_return_false_when_value_is_out_of_range(void)
{
    TEST_ASSERT_FALSE(cobs_bar(-1));
    TEST_ASSERT_FALSE(cobs_bar(101));
    TEST_ASSERT_FALSE(cobs_bar(-100));
    TEST_ASSERT_FALSE(cobs_bar(200));
}

/* Test Cases for cobsFactorial */

void test_factorial_should_return_correct_value_when_input_is_zero(void)
{
    cobs_result_t result = cobs_factorial(0);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_one(void)
{
    cobs_result_t result = cobs_factorial(1);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_five(void)
{
    cobs_result_t result = cobs_factorial(5);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(120, result.value);
}

void test_factorial_should_return_correct_value_when_input_is_ten(void)
{
    cobs_result_t result = cobs_factorial(10);
    TEST_ASSERT_EQUAL(COBS_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(3628800, result.value);
}

void test_factorial_should_return_error_invalid_when_input_is_negative(void)
{
    cobs_result_t result = cobs_factorial(-1);
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

void test_factorial_should_return_error_invalid_when_input_is_too_large(void)
{
    cobs_result_t result = cobs_factorial(13);
    TEST_ASSERT_EQUAL(COBS_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

/* Main Test Runner */

int main(void)
{
    UNITY_BEGIN();

    /* Version tests */
    RUN_TEST(test_get_version_should_return_version_string);

    /* Add function tests */
    RUN_TEST(test_add_should_return_sum_when_adding_positive_numbers);
    RUN_TEST(test_add_should_return_sum_when_adding_negative_numbers);
    RUN_TEST(test_add_should_return_sum_when_adding_mixed_numbers);
    RUN_TEST(test_add_should_return_zero_when_adding_zeros);
    RUN_TEST(test_add_should_saturate_on_overflow);
    RUN_TEST(test_add_should_saturate_on_underflow);

    /* Multiply function tests */
    RUN_TEST(test_multiply_should_return_success_when_multiplying_positive_numbers);
    RUN_TEST(test_multiply_should_return_success_when_multiplying_by_zero);
    RUN_TEST(test_multiply_should_return_error_null_when_result_pointer_is_null);
    RUN_TEST(test_multiply_should_return_error_invalid_on_overflow);
    RUN_TEST(test_multiply_should_return_error_invalid_on_underflow);

    /* Foo function tests */
    RUN_TEST(test_foo_should_return_success_when_processing_valid_input);
    RUN_TEST(test_foo_should_return_error_null_when_input_is_null);
    RUN_TEST(test_foo_should_return_error_null_when_output_is_null);
    RUN_TEST(test_foo_should_return_error_invalid_when_output_size_is_zero);
    RUN_TEST(test_foo_should_return_error_invalid_when_buffer_too_small);
    RUN_TEST(test_foo_should_succeed_when_buffer_exact_size);
    RUN_TEST(test_foo_should_return_error_invalid_when_buffer_one_byte_short);

    /* Bar function tests */
    RUN_TEST(test_bar_should_return_true_when_value_is_in_range);
    RUN_TEST(test_bar_should_return_false_when_value_is_out_of_range);

    /* Factorial function tests */
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_zero);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_one);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_five);
    RUN_TEST(test_factorial_should_return_correct_value_when_input_is_ten);
    RUN_TEST(test_factorial_should_return_error_invalid_when_input_is_negative);
    RUN_TEST(test_factorial_should_return_error_invalid_when_input_is_too_large);

    return UNITY_END();
}
