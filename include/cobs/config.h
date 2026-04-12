/* config.h - Zephyr build configuration for COBS library */

/* All Rights Reserved */

#ifndef INC_COBS_CONFIG_H
#define INC_COBS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#define COBS_STATIC

/* On-wire fragment envelope (per single COBS-encoded fragment). Sized to
 * fit in one BLE notification at MTU 247 (247 - 3 ATT header = 244). Also
 * sizes the per-context RX accumulator and TX scratch buffers. Values come
 * from Kconfig when building under Zephyr; otherwise fall back. */
#ifdef CONFIG_COBS_FRAME_SIZE
#define COBS_FRAME_SIZE (CONFIG_COBS_FRAME_SIZE)
#elif !defined(COBS_FRAME_SIZE)
#define COBS_FRAME_SIZE (244)
#endif

/* Upper bound on a single reassembled user message. Sizes the reassembly
 * buffer inside each cobs_framing_t context. Large messages are split into
 * multiple fragments automatically. */
#ifdef CONFIG_COBS_MAX_MESSAGE_SIZE
#define COBS_MAX_MESSAGE_SIZE (CONFIG_COBS_MAX_MESSAGE_SIZE)
#elif !defined(COBS_MAX_MESSAGE_SIZE)
#define COBS_MAX_MESSAGE_SIZE (4096)
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_COBS_CONFIG_H */
