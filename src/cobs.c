/// @file cobs.c
/// @author Dogukan Fikri Arat
/// @brief Implementation of the cobs module
/// @date 2026-04-05

/* Includes */

#include "cobs/cobs.h"

#include <errno.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cobs, CONFIG_COBS_LOG_LEVEL);

/* Imports */


/* Definitions */

#define COBS_MAX_CODE_VALUE  0xFF
#define COBS_CODE_BLOCK_SIZE (COBS_MAX_CODE_VALUE - 1)

/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */


static uint16_t cobs_crc16_update (uint16_t crc, uint8_t byte)
{
    uint8_t i;

    crc ^= (uint16_t)byte << 8;
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x8000)
        {
            crc = (crc << 1) ^ COBS_CRC16_POLY;
        }
        else
        {
            crc <<= 1;
        }
    }
    return crc;
}

uint16_t cobs_crc16_ccitt (const uint8_t *data, size_t len)
{
    uint16_t crc = COBS_CRC16_INIT;
    for (size_t i = 0; i < len; i++)
    {
        crc = cobs_crc16_update(crc, data[i]);
    }
    return crc;
}

/**
 * @brief Internal helper to encode a contiguous block of memory into COBS.
 * This version allows us to "chain" the payload and CRC without a temporary buffer.
 */
static int32_t cobs_encode_step (
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t output_size,
    size_t *read_idx,
    size_t *write_idx,
    size_t *code_idx,
    uint8_t *code,
    bool is_final_step)
{
    while (*read_idx < input_len)
    {
        if (*write_idx >= output_size)
        {
            return -EOVERFLOW;
        }

        if (input[*read_idx] == 0x00)
        {
            output[*code_idx] = *code;
            *code = 1;
            *code_idx = (*write_idx)++;
            (*read_idx)++;
        }
        else
        {
            output[(*write_idx)++] = input[(*read_idx)++];
            (*code)++;
            if (*code == 0xFF)
            {
                output[*code_idx] = *code;
                *code = 1;
                *code_idx = (*write_idx)++;
            }
        }
    }

    if (is_final_step)
    {
        if (*code_idx >= output_size)
        {
            return -EOVERFLOW;
        }
        output[*code_idx] = *code;
    }
    return 0;
}

static int32_t cobs_decode (
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t output_size,
    size_t *decoded_len_out)
{
    size_t read_idx = 0;
    size_t write_idx = 0;
    while (read_idx < input_len)
    {
        uint8_t code = input[read_idx++];
        if (code == 0x00)
        {
            return -EBADMSG;
        }

        uint8_t copy = code - 1;
        if (read_idx + copy > input_len || write_idx + copy > output_size)
        {
            return -EOVERFLOW;
        }

        for (uint8_t i = 0; i < copy; i++)
        {
            output[write_idx++] = input[read_idx++];
        }

        if (code < 0xFF && read_idx < input_len)
        {
            if (write_idx >= output_size)
            {
                return -EOVERFLOW;
            }
            output[write_idx++] = 0x00;
        }
    }
    *decoded_len_out = write_idx;
    return 0;
}

int32_t cobs_rx_init (cobs_rx_context_t *ctx, uint8_t *buffer, size_t buffer_size)
{
    if (!ctx || !buffer || buffer_size == 0)
    {
        return -EINVAL;
    }
    ctx->buffer = buffer;
    ctx->buffer_size = buffer_size;
    return cobs_rx_reset(ctx);
}

int32_t cobs_rx_reset (cobs_rx_context_t *ctx)
{
    if (!ctx || !ctx->buffer || ctx->buffer_size == 0)
    {
        return -EINVAL;
    }
    ctx->state = COBS_RX_STATE_IDLE;
    ctx->frame_len = 0;
    return 0;
}

bool cobs_rx_process_byte (cobs_rx_context_t *ctx, uint8_t byte)
{
    if (byte == COBS_FRAME_DELIMITER)
    {
        // Treat any delimiter as a frame boundary
        bool frame_ready = (ctx->state == COBS_RX_STATE_RECEIVING && ctx->frame_len > 0);
        ctx->state = frame_ready ? COBS_RX_STATE_COMPLETE : COBS_RX_STATE_RECEIVING;
        if (!frame_ready)
        {
            ctx->frame_len = 0;
        }
        return frame_ready;
    }

    if (ctx->state == COBS_RX_STATE_RECEIVING)
    {
        if (ctx->frame_len < ctx->buffer_size)
        {
            ctx->buffer[ctx->frame_len++] = byte;
        }
        else
        {
            cobs_rx_reset(ctx); // Overflow: drop and wait for next delimiter
        }
    }
    return false;
}

int32_t cobs_rx_get_frame (
    cobs_rx_context_t *ctx,
    const uint8_t **frame_data_out,
    size_t *frame_len_out)
{
    if (ctx->state != COBS_RX_STATE_COMPLETE)
    {
        return -EBUSY;
    }

    LOG_DBG("Decoding COBS frame len=%u", ctx->frame_len);
    LOG_HEXDUMP_DBG(ctx->buffer, ctx->frame_len, "cobs_rx");

    size_t decoded_len;
    int32_t status = cobs_decode(
        ctx->buffer,
        ctx->frame_len,
        ctx->buffer,
        ctx->buffer_size,
        &decoded_len);

    // Always reset state after a get_frame attempt to prepare for next
    cobs_rx_reset(ctx);

    if (status != 0)
    {
        return status;
    }
    if (decoded_len < COBS_FCS_SIZE)
    {
        return -EMSGSIZE;
    }

    size_t payload_len = decoded_len - COBS_FCS_SIZE;
    uint16_t received_crc = ((uint16_t)ctx->buffer[payload_len + 1] << 8) |
                            ctx->buffer[payload_len];
    uint16_t calculated_crc = cobs_crc16_ccitt(ctx->buffer, payload_len);

    if (received_crc != calculated_crc)
    {
        return -EILSEQ;
    }

    *frame_data_out = ctx->buffer;
    *frame_len_out = payload_len;
    return 0;
}

int32_t cobs_tx_encode_frame (
    const uint8_t *payload,
    size_t payload_len,
    uint8_t *output,
    size_t output_size,
    size_t *encoded_len_out)
{
    if (!payload || !output || !encoded_len_out)
    {
        return -EINVAL;
    }

    uint16_t crc = cobs_crc16_ccitt(payload, payload_len);
    uint8_t crc_bytes[2] = {(uint8_t)(crc & 0xFF), (uint8_t)(crc >> 8)};

    size_t write_idx = 2; // [0] = Delimiter, [1] = First COBS code
    size_t code_idx = 1;
    size_t read_idx = 0;
    uint8_t code = 1;

    if (output_size < (payload_len + (payload_len / COBS_CODE_BLOCK_SIZE) + 5))
    {
        return -EOVERFLOW;
    }

    output[0] = COBS_FRAME_DELIMITER;

    // Step 1: Encode Payload
    cobs_encode_step(
        payload,
        payload_len,
        output,
        output_size - 1,
        &read_idx,
        &write_idx,
        &code_idx,
        &code,
        false);

    // Step 2: Encode CRC
    read_idx = 0;
    cobs_encode_step(
        crc_bytes,
        2,
        output,
        output_size - 1,
        &read_idx,
        &write_idx,
        &code_idx,
        &code,
        true);

    // Step 3: End Delimiter
    output[write_idx++] = COBS_FRAME_DELIMITER;

    *encoded_len_out = write_idx;

    LOG_DBG("Encoded COBS frame len=%u", *encoded_len_out);
    LOG_HEXDUMP_DBG(output, *encoded_len_out, "cobs_tx");

    return 0;
}
