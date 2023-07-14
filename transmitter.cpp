#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "tx.pio.h"
#include"transmitter.h"
#define LED_COUNT 11
static uint32_t colours[LED_COUNT] = 
{
    0x00ACCC,
    0x1D5DC5,
    0x541ECB,
    0x9C1CEE,
    0xB30D8B,
    0xBC1303,
    0xD6AB01,
    0xBEFE00,
    0x69FF03,
    0x26FF00,
    0x00FF19
};

const uint8_t OUTPUT_PIN=15;
uint32_t led_buffer[LED_COUNT];
#define LED_COUNT 11
void ws2812a_program_init(PIO pio, uint sm, uint offset, uint pin)
{
    pio_sm_config cfg = ws2812a_program_get_default_config(offset);
    sm_config_set_out_shift(&cfg, false, false, 32);
    sm_config_set_clkdiv(&cfg, (clock_get_hz(clk_sys) / 1e6f) * 2.5f / 5.0f);    // 1.25us / 3
    sm_config_set_out_pins(&cfg, pin, 1);

    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_init(pio, sm, offset, &cfg);
}
transmitter::transmitter()
{
    const uint32_t outOffset = pio_add_program(outPio, &ws2812a_program);
    ws2812a_program_init(outPio, outSm, outOffset, OUTPUT_PIN);

// configure DMA to copy the LED buffer to the PIO state machine's FIFO
    dma_ch0 = dma_channel_get_default_config(outDmaIndex);
    channel_config_set_transfer_data_size(&dma_ch0, DMA_SIZE_32);
    channel_config_set_read_increment(&dma_ch0, false);
    channel_config_set_write_increment(&dma_ch0, false);
    channel_config_set_dreq(&dma_ch0, DREQ_PIO0_TX0);
    pio_sm_set_enabled(outPio, outSm, true);
    gpio_put(OUTPUT_PIN, false);

}
void transmitter::transmit(uint32_t data)
{
    buffer=data;
        // initiate DMA transfer 
        dma_channel_configure(outDmaIndex, &dma_ch0, &outPio->txf[outSm], &buffer,1,true);

}
