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
#include "cobs/export.h"
#include "cobs/version.h"
#include "cobs/cobs_types.h"

/* Configurations */


/* Definitions */

#define COBS_FRAME_DELIMITER 0x00   ///< Frame boundary delimiter */
#define COBS_FCS_SIZE        2      ///< Frame Check Sequence size in bytes */
#define COBS_CRC16_POLY      0x1021 ///< CRC polynomial */
#define COBS_CRC16_INIT      0xFFFF ///< Initial CRC value */

/* Types */

/// @brief State machine states for Cobs frame reception.
typedef enum cobs_rx_state_s
{
    COBS_RX_STATE_IDLE,      ///< Waiting for frame start */
    COBS_RX_STATE_RECEIVING, ///< Accumulating frame data */
    COBS_RX_STATE_COMPLETE   ///< Frame ready for validation */
} cobs_rx_state_t;

/// @brief Context structure for Cobs frame reception.
typedef struct cobs_rx_context_s
{
    cobs_rx_state_t state; ///< Current state machine state */
    uint8_t *buffer;       ///< Frame accumulation buffer */
    size_t buffer_size;    ///< Maximum buffer capacity */
    size_t frame_len;      ///< Current encoded frame length */
} cobs_rx_context_t;

/* External Declarations */

/// @brief Initializes the RX context for Cobs frame reception.
/// @param[in] ctx Pointer to the RX context to initialize.
/// @param[in] buffer Pointer to the buffer for accumulating received frame data.
/// @param[in] buffer_size Size of the provided buffer in bytes.
/// @return PS_SUCCESS or negative int32_t on failure.
COBS_API int32_t cobs_rx_init (cobs_rx_context_t *ctx, uint8_t *buffer, size_t buffer_size);

/// @brief Resets the RX context to its initial state, clearing any accumulated data and resetting the state machine.
/// @param[in] ctx Pointer to the RX context to reset.
/// @return PS_SUCCESS or negative int32_t on failure.
COBS_API int32_t cobs_rx_reset (cobs_rx_context_t *ctx);

/// @brief Processes a single byte of incoming data for Cobs frame reception, updating the RX context state machine accordingly.
/// @param[in] ctx Pointer to the RX context to update.
/// @param[in] byte The incoming byte to process.
/// @return true if a complete frame has been received and is ready for validation, false otherwise
COBS_API bool cobs_rx_process_byte (cobs_rx_context_t *ctx, uint8_t byte);

/// @brief Retrieves the complete frame data from the RX context after a successful reception, along with its length.
/// @param[in] ctx Pointer to the RX context containing the received frame.
/// @param[out] frame_data_out Pointer to a variable that will receive the pointer to the complete frame data within the RX context's buffer.
/// @param[out] frame_len_out Pointer to a variable that will receive the length of the complete frame data in bytes.
/// @return PS_SUCCESS or negative int32_t on failure (e.g., if the RX context is not in a valid state for retrieving the frame).
COBS_API int32_t
cobs_rx_get_frame (cobs_rx_context_t *ctx, const uint8_t **frame_data_out, size_t *frame_len_out);

/// @brief Encodes a payload into a Cobs frame, including the Frame Check Sequence (FCS), and writes the encoded frame to the provided output buffer.
/// @param[in] payload Pointer to the input data to encode.
/// @param[in] payload_len Length of the input data in bytes.
/// @param[out] output Pointer to the buffer where the encoded frame will be written.
/// @param[in] output_size Size of the output buffer in bytes.
/// @param[out] encoded_len_out Pointer to a variable that will receive the length of the encoded frame in bytes.
/// @return PS_SUCCESS or negative int32_t on failure (e.g., if the output buffer is too small to hold the encoded frame).
COBS_API int32_t cobs_tx_encode_frame (
    const uint8_t *payload,
    size_t payload_len,
    uint8_t *output,
    size_t output_size,
    size_t *encoded_len_out);

/// @brief Validates a received Cobs frame by checking its Frame Check Sequence (FCS) against the expected value computed from the frame data.
/// @param[in] data Pointer to the complete frame data, including the FCS, to validate.
/// @param[in] len Length of the complete frame data in bytes.
/// @return crc16_t The computed CRC16-CCITT value for the provided data, which can be compared against the FCS in the frame for validation.
COBS_API uint16_t cobs_crc16_ccitt (const uint8_t *data, size_t len);


#ifdef __cplusplus
}
#endif

#endif /* INC_COBS_H */
