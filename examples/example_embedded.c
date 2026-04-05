/* example_embedded.c - Embedded-safe example for cobs */

/* All Rights Reserved */

/*
 * Minimal example that avoids POSIX APIs for embedded/OSAL environments.
 * This is intended to compile on bare-metal or RTOS targets.
 */

/* Includes */

#include "cobs/cobs.h"
#include <stddef.h>
#include <stdint.h>

/* Definitions */

#define OUTPUT_BUFFER_SIZE 32

/* Types */

/* Forward Declarations */

/* Functions */

int main(void)
{
    int32_t multiplyResult = 0;
    char output[OUTPUT_BUFFER_SIZE];

    (void)cobs_get_version();

    (void)cobs_add(10, 20);
    (void)cobs_multiply(3, 7, &multiplyResult);
    (void)cobs_foo("embedded", output, sizeof(output));
    (void)cobs_bar(42);
    (void)cobs_factorial(5);

    (void)multiplyResult;
    (void)output;

    return 0;
}
