/* cobs.h - Main API header for cobs library */

/* All Rights Reserved */

#ifndef INC_COBS_H
#define INC_COBS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdbool.h>
#include <stddef.h>

#include "cobs/config.h"
#include "cobs/version.h"
#include "cobs/export.h"
#include "cobs/cobs_types.h"

/* Configurations */


/* Definitions */


/* Types */


/* External Declarations */

/**
 * @brief Get library version string
 *
 * @return Version string in format "major.minor.patch"
 */
COBS_API const char *cobs_get_version(void);

/**
 * @brief Add two integers with saturation on overflow/underflow
 *
 * @param a First operand
 * @param b Second operand
 * @return Saturated sum of a and b
 */
COBS_API int32_t cobs_add(int32_t a, int32_t b);

/**
 * @brief Multiply two integers with error handling
 *
 * @param a First operand
 * @param b Second operand
 * @param result Pointer to store result
 * @return COBS_SUCCESS on success, error code otherwise
 */
COBS_API cobs_status_t cobs_multiply(
    int32_t a,
    int32_t b,
    int32_t *result);

/**
 * @brief Example function that processes a string
 *
 * @param input Input string to process
 * @param output Buffer to store processed string
 * @param outputSize Size of output buffer
 * @return COBS_SUCCESS on success, error code otherwise
 */
COBS_API cobs_status_t cobs_foo(
    const char *input,
    char *output,
    size_t outputSize);

/**
 * @brief Example function that validates input
 *
 * @param value Value to validate
 * @return true if valid, false otherwise
 */
COBS_API bool cobs_bar(int32_t value);

/**
 * @brief Compute factorial of a number
 *
 * @param n Input number (must be >= 0 and <= 12)
 * @return Result structure with factorial value and status
 */
COBS_API cobs_result_t cobs_factorial(int32_t n);

#ifdef __cplusplus
}
#endif

#endif /* INC_COBS_H */
