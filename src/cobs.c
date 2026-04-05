/* cobs.c - Core implementation of cobs library */

/* All Rights Reserved */

/* Includes */

#include "cobs/cobs.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Imports */


/* Definitions */

#define MAX_FACTORIAL_INPUT 12

/* Version string helpers */
#define COBS_STRINGIFY_HELPER(x) #x
#define COBS_STRINGIFY(x) COBS_STRINGIFY_HELPER(x)

static const char VersionString[] = COBS_STRINGIFY(COBS_VERSION_MAJOR) "."
                                   COBS_STRINGIFY(COBS_VERSION_MINOR) "."
                                   COBS_STRINGIFY(COBS_VERSION_PATCH);

/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */

const char *cobs_get_version(void)
{
    return VersionString;
}

int32_t cobs_add(int32_t a, int32_t b)
{
    int64_t sum = (int64_t)a + (int64_t)b;

    if (sum > INT32_MAX)
    {
        return INT32_MAX;
    }

    if (sum < INT32_MIN)
    {
        return INT32_MIN;
    }

    return (int32_t)sum;
}

cobs_status_t cobs_multiply(
    int32_t a,
    int32_t b,
    int32_t *result)
{
    /* Validate input parameter */
    if (result == NULL)
    {
        return COBS_ERROR_NULL;
    }

    /* Perform multiplication */
    int64_t product = (int64_t)a * (int64_t)b;
    if (product > INT32_MAX || product < INT32_MIN)
    {
        return COBS_ERROR_INVALID;
    }

    *result = (int32_t)product;

    return COBS_SUCCESS;
}

cobs_status_t cobs_foo(
    const char *input,
    char *output,
    size_t outputSize)
{
    /* Validate input parameters */
    if (input == NULL || output == NULL)
    {
        return COBS_ERROR_NULL;
    }

    if (outputSize == 0)
    {
        return COBS_ERROR_INVALID;
    }

    /* Get input length */
    size_t inputLen = strlen(input);

    const char prefix[] = "Processed: ";
    size_t prefixLen = sizeof(prefix) - 1;

    /* Check if output buffer is large enough */
    if (outputSize <= prefixLen || inputLen > (outputSize - prefixLen - 1))
    {
        return COBS_ERROR_INVALID;
    }

    /* Process the string by adding a prefix */
    int written = snprintf(output, outputSize, "%s%s", prefix, input);
    if (written < 0 || (size_t)written >= outputSize)
    {
        return COBS_ERROR_INVALID;
    }

    return COBS_SUCCESS;
}

bool cobs_bar(int32_t value)
{
    /* Example validation: value must be in range [0, 100] */
    return (value >= 0 && value <= 100);
}

cobs_result_t cobs_factorial(int32_t n)
{
    cobs_result_t result;

    /* Validate input */
    if (n < 0)
    {
        result.value = 0;
        result.status = COBS_ERROR_INVALID;
        return result;
    }

    /* Check for overflow protection */
    if (n > MAX_FACTORIAL_INPUT)
    {
        result.value = 0;
        result.status = COBS_ERROR_INVALID;
        return result;
    }

    /* Calculate factorial */
    int32_t factorial = 1;
    for (int32_t i = 2; i <= n; i++)
    {
        factorial *= i;
    }

    result.value = factorial;
    result.status = COBS_SUCCESS;

    return result;
}
