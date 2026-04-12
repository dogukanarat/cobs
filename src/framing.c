/// @file framing.c
/// @author Dogukan Fikri Arat
/// @brief Implementation of the framing module
/// @date 2026-04-05

/* Includes */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "osal/osal_types.h"

#include "cobs/cobs.h"
#include "cobs/framing.h"

/* Imports */


/* Definitions */

/* Internal signal: a fragment was consumed but the full message is not yet
 * reassembled. Positive so it sorts cleanly above zero-success without
 * colliding with any negative errno. */
#define COBS_FRAMING_INCOMPLETE (1)

/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */

static int32_t cobs_framing_tx_init (cobs_framing_tx_t *ctx, const uint8_t *payload, size_t len)
{
    int32_t rc = -EINVAL;
    size_t total;

    for (;;)
    {
        if (ctx == NULL || payload == NULL || len == 0)
        {
            break;
        }

        if (len > COBS_MAX_MESSAGE_SIZE)
        {
            rc = -EMSGSIZE;
            break;
        }

        total = (len + COBS_FRAMING_MAX_PAYLOAD - 1) / COBS_FRAMING_MAX_PAYLOAD;

        if (total > 255)
        {
            rc = -EMSGSIZE;
            break;
        }

        ctx->payload = payload;
        ctx->payload_len = len;
        ctx->offset = 0;
        ctx->seq = 0;
        ctx->total = (uint8_t)total;

        rc = 0;
        break;
    }

    return rc;
}

static int32_t cobs_framing_tx_next (
    cobs_framing_tx_t *ctx,
    uint8_t *output,
    size_t output_size,
    size_t *encoded_len
)
{
    int32_t rc = -EINVAL;
    uint8_t temp[COBS_FRAMING_HEADER_SIZE + COBS_FRAMING_MAX_PAYLOAD];
    size_t chunk_len;
    size_t remaining;
    uint8_t flags;

    for (;;)
    {
        if (ctx == NULL || output == NULL || encoded_len == NULL)
        {
            break;
        }

        if (ctx->seq >= ctx->total)
        {
            rc = -ENODATA;
            break;
        }

        remaining = ctx->payload_len - ctx->offset;
        chunk_len = remaining < COBS_FRAMING_MAX_PAYLOAD ? remaining : COBS_FRAMING_MAX_PAYLOAD;

        flags = 0;
        if (ctx->seq == 0)
        {
            flags |= COBS_FRAMING_FIRST;
        }
        if (ctx->seq == ctx->total - 1)
        {
            flags |= COBS_FRAMING_LAST;
        }

        temp[0] = flags;
        temp[1] = ctx->seq;
        temp[2] = ctx->total;
        memcpy(&temp[COBS_FRAMING_HEADER_SIZE], &ctx->payload[ctx->offset], chunk_len);

        rc = cobs_tx_encode_frame(
            temp, COBS_FRAMING_HEADER_SIZE + chunk_len, output, output_size, encoded_len
        );
        if (rc < 0)
        {
            break;
        }

        ctx->offset += chunk_len;
        ctx->seq++;

        rc = 0;
        break;
    }

    return rc;
}

static void cobs_framing_rx_reset (cobs_framing_rx_t *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    ctx->total_len = 0;
    ctx->expected_seq = 0;
    ctx->total = 0;
    ctx->active = false;
}

/* Absorb one decoded COBS fragment into the parent framing context's
 * reassembly buffer. Returns 0 when the message is complete (LAST seen),
 * COBS_FRAMING_INCOMPLETE when more fragments are expected, or a negative
 * errno on protocol violations. On any error or completion, the caller is
 * responsible for consulting ctx->frag_rx.total_len / reasm_buf. */
static int32_t cobs_framing_rx_process (cobs_framing_t *ctx, const uint8_t *frame, size_t len)
{
    int32_t rc = -EINVAL;
    uint8_t flags;
    uint8_t seq;
    uint8_t total;
    size_t chunk_len;

    for (;;)
    {
        if (ctx == NULL || frame == NULL)
        {
            break;
        }

        if (len < COBS_FRAMING_HEADER_SIZE)
        {
            rc = -EBADMSG;
            break;
        }

        flags = frame[0];
        seq = frame[1];
        total = frame[2];
        chunk_len = len - COBS_FRAMING_HEADER_SIZE;

        if (flags & COBS_FRAMING_FIRST)
        {
            cobs_framing_rx_reset(&ctx->frag_rx);
            ctx->frag_rx.active = true;
            ctx->frag_rx.total = total;
        }

        if (!ctx->frag_rx.active)
        {
            rc = -EPROTO;
            break;
        }

        if (seq != ctx->frag_rx.expected_seq || total != ctx->frag_rx.total)
        {
            cobs_framing_rx_reset(&ctx->frag_rx);
            rc = -EBADMSG;
            break;
        }

        if (ctx->frag_rx.total_len + chunk_len > COBS_MAX_MESSAGE_SIZE)
        {
            cobs_framing_rx_reset(&ctx->frag_rx);
            rc = -EMSGSIZE;
            break;
        }

        memcpy(&ctx->reasm_buf[ctx->frag_rx.total_len],
               &frame[COBS_FRAMING_HEADER_SIZE],
               chunk_len);

        ctx->frag_rx.total_len += chunk_len;
        ctx->frag_rx.expected_seq++;

        if (flags & COBS_FRAMING_LAST)
        {
            rc = 0;
        }
        else
        {
            rc = COBS_FRAMING_INCOMPLETE;
        }

        break;
    }

    return rc;
}

/* --- Public Functions --- */

int32_t cobs_framing_init (cobs_framing_t *ctx, const cobs_framing_config_t *config)
{
    int32_t rc = -EINVAL;

    for (;;)
    {
        if (ctx == NULL || config == NULL)
        {
            break;
        }

        if (config->send_frame_cb == NULL || config->on_frame_decoded_cb == NULL)
        {
            break;
        }

        ctx->config = *config;

        rc = cobs_rx_init(&ctx->rx_ctx, ctx->rx_buf, sizeof(ctx->rx_buf));
        if (rc < 0)
        {
            break;
        }

        cobs_framing_rx_reset(&ctx->frag_rx);

        rc = 0;
        break;
    }

    return rc;
}

int32_t cobs_framing_deinit (cobs_framing_t *ctx)
{
    int32_t rc = -EINVAL;

    for (;;)
    {
        if (ctx == NULL)
        {
            break;
        }

        cobs_framing_rx_reset(&ctx->frag_rx);

        rc = 0;
        break;
    }

    return rc;
}

int32_t cobs_framing_send (cobs_framing_t *ctx, const uint8_t *data, size_t len)
{
    int32_t rc = -EINVAL;
    cobs_framing_tx_t frag_tx;
    size_t encoded_len = 0;

    for (;;)
    {
        if (ctx == NULL || data == NULL || len == 0)
        {
            break;
        }

        rc = cobs_framing_tx_init(&frag_tx, data, len);
        if (rc < 0)
        {
            break;
        }

        while (frag_tx.seq < frag_tx.total)
        {
            rc = cobs_framing_tx_next(&frag_tx, ctx->tx_buf, sizeof(ctx->tx_buf), &encoded_len);
            if (rc < 0)
            {
                break;
            }

            rc = ctx->config.send_frame_cb(
                ctx->tx_buf, encoded_len, ctx->config.send_frame_cb_arg
            );
            if (rc < 0)
            {
                break;
            }
        }

        break;
    }

    return rc;
}

int32_t cobs_framing_received (cobs_framing_t *ctx, const uint8_t *data, size_t len)
{
    int32_t rc = -EINVAL;
    size_t i;
    const uint8_t *frame_data = NULL;
    size_t frame_len = 0;
    int32_t frag_rc = 0;
    size_t total_len;

    for (;;)
    {
        if (ctx == NULL || data == NULL || len == 0)
        {
            break;
        }

        rc = 0;

        for (i = 0; i < len; i++)
        {
            if (!cobs_rx_process_byte(&ctx->rx_ctx, data[i]))
            {
                continue;
            }

            rc = cobs_rx_get_frame(&ctx->rx_ctx, &frame_data, &frame_len);
            if (rc < 0)
            {
                cobs_rx_reset(&ctx->rx_ctx);
                break;
            }

            frag_rc = cobs_framing_rx_process(ctx, frame_data, frame_len);
            cobs_rx_reset(&ctx->rx_ctx);

            if (frag_rc == 0)
            {
                total_len = ctx->frag_rx.total_len;
                /* Reset state BEFORE the callback so that nested RX
                 * activity (e.g. if the callback turns around and calls
                 * cobs_framing_received) sees a clean reassembly slot.
                 * The callback is still handed a valid pointer/length
                 * because reasm_buf survives the reset — only the
                 * tracking fields are cleared. */
                cobs_framing_rx_reset(&ctx->frag_rx);

                rc = ctx->config.on_frame_decoded_cb(
                    ctx->reasm_buf, total_len, ctx->config.on_frame_decoded_cb_arg
                );
                if (rc < 0)
                {
                    break;
                }
            }
            else if (frag_rc == COBS_FRAMING_INCOMPLETE)
            {
                /* Still receiving more fragments; keep looping. */
                continue;
            }
            else
            {
                cobs_framing_rx_reset(&ctx->frag_rx);
                rc = frag_rc;
                break;
            }
        }

        break;
    }

    return rc;
}
