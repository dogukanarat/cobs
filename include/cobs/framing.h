/// @file framing.h
/// @author Dogukan Fikri Arat
/// @brief COBS framing module
/// @date 2026-04-05

#ifndef FRAMING_H
#define FRAMING_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cobs/cobs.h"
#include "cobs/config.h"
#include "cobs/export.h"

/* Configurations */


/* Definitions */

#define COBS_FRAMING_HEADER_SIZE 3    ///< Fragment header: flags + sequence + total
#define COBS_FRAMING_FIRST       0x01 ///< Flag: first fragment
#define COBS_FRAMING_LAST        0x02 ///< Flag: last fragment

/// @brief Maximum user-data payload per fragment.
/// Sized to fit when COBS-encoded into COBS_FRAME_SIZE.
/// The -5 accounts for worst-case COBS encoding overhead (code bytes + CRC + delimiters).
#define COBS_FRAMING_MAX_PAYLOAD (COBS_FRAME_SIZE - COBS_FRAMING_HEADER_SIZE - 5)

/* Types */

/// @brief TX fragmentation context (iterator pattern).
typedef struct cobs_framing_tx_s
{
    const uint8_t *payload; ///< Pointer to the full payload
    size_t payload_len;     ///< Total payload length
    size_t offset;          ///< Current offset into payload
    uint8_t seq;            ///< Current fragment sequence number
    uint8_t total;          ///< Total number of fragments
} cobs_framing_tx_t;

/// @brief RX fragment reassembly state — fragments are written directly
/// into the owning cobs_framing_t's reasm_buf at total_len offset.
typedef struct cobs_framing_rx_s
{
    size_t total_len;     ///< Bytes written into reasm_buf so far
    uint8_t expected_seq; ///< Next expected sequence number
    uint8_t total;        ///< Expected total fragments
    bool active;          ///< Whether reassembly is in progress
} cobs_framing_rx_t;

/// @brief Callback: send an encoded frame to the driver.
typedef int32_t (*cobs_framing_send_cb_t)(const uint8_t *frame, size_t len, void *user);

/// @brief Callback: deliver a fully reassembled message to the application.
/// The buffer is owned by the framing context and is only valid for the
/// duration of the callback. If the callback needs to keep the data, it
/// MUST copy it before returning — subsequent RX activity will reuse the
/// underlying reassembly buffer.
typedef int32_t (*cobs_framing_receive_cb_t)(uint8_t *data, size_t len, void *user);

/// @brief Configuration for cobs_framing_init.
typedef struct cobs_framing_config_s
{
    cobs_framing_send_cb_t send_frame_cb;          ///< Driver send callback (TX path)
    cobs_framing_receive_cb_t on_frame_decoded_cb; ///< Application receive callback (RX path)
    void *send_frame_cb_arg;                       ///< Opaque pointer passed to send callback
    void *on_frame_decoded_cb_arg;                 ///< Opaque pointer passed to receive callback
} cobs_framing_config_t;

/// @brief Unified framing context — owns all internal state.
typedef struct cobs_framing_s
{
    cobs_framing_config_t config;
    cobs_rx_context_t rx_ctx;
    cobs_framing_rx_t frag_rx;
    uint8_t rx_buf[COBS_FRAME_SIZE];          ///< COBS RX accumulation buffer
    uint8_t tx_buf[COBS_FRAME_SIZE];          ///< Scratch buffer for encoded frames
    uint8_t reasm_buf[COBS_MAX_MESSAGE_SIZE]; ///< Multi-fragment reassembly buffer
} cobs_framing_t;

/* External Declarations */

/// @brief Initializes the framing context with the provided configuration.
/// @param[in] ctx    Pointer to the framing context to initialize.
/// @param[in] config Pointer to the configuration (callbacks and user pointer).
/// @return PS_SUCCESS on success, or an appropriate error code on failure.
COBS_API int32_t cobs_framing_init (cobs_framing_t *ctx, const cobs_framing_config_t *config);

/// @brief Deinitializes the framing context, releasing all internal resources.
/// @param[in] ctx Pointer to the framing context to deinitialize.
/// @return PS_SUCCESS on success, or an appropriate error code on failure.
COBS_API int32_t cobs_framing_deinit (cobs_framing_t *ctx);

/// @brief Sends a message by fragmenting, encoding, and invoking the send callback per frame.
/// @param[in] ctx  Pointer to the framing context.
/// @param[in] data Pointer to the message payload to send.
/// @param[in] len  Length of the message payload in bytes.
/// @return PS_SUCCESS on success, or an appropriate error code on failure.
COBS_API int32_t cobs_framing_send (cobs_framing_t *ctx, const uint8_t *data, size_t len);

/// @brief Processes received raw bytes, decoding COBS frames and reassembling fragments.
///        When a complete message is reassembled, the receive callback is invoked.
/// @param[in] ctx  Pointer to the framing context.
/// @param[in] data Pointer to the received raw byte data.
/// @param[in] len  Length of the received data in bytes.
/// @return PS_SUCCESS on success, or an appropriate error code on failure.
COBS_API int32_t cobs_framing_received (cobs_framing_t *ctx, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FRAMING_H */