/* cobs_types.h - Common types and definitions */

/* All Rights Reserved */

#ifndef INC_COBS_TYPES_H
#define INC_COBS_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdint.h>

/* Configurations */


/* Definitions */


/* Types */

/**
 * @brief Return status codes for library functions
 */
typedef enum
{
    COBS_SUCCESS = 0,     /**< Operation successful */
    COBS_ERROR = -1,      /**< Generic error */
    COBS_ERROR_NULL = -2, /**< Null pointer error */
    COBS_ERROR_INVALID = -3 /**< Invalid parameter error */
} cobs_status_t;

/**
 * @brief Operation result structure
 */
typedef struct
{
    int32_t value;               /**< Result value */
    cobs_status_t status; /**< Operation status */
} cobs_result_t;

/* External Declarations */


#ifdef __cplusplus
}
#endif

#endif /* INC_COBS_TYPES_H */
