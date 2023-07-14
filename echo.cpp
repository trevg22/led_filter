#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "rx.pio.h"
#include "tx.pio.h"

#include <stdio.h>

// uncomment one of these to use its implementation
#define USE_WS2812A_PIO
// #define USE_WS2812B_PIO
// #define USE_WS2812C_PIO
// #define USE_WS2812D_PIO

#define WS2812_PIN 15
const uint8_t OUTPUT_PIN = 15;
const uint8_t INPUT_PIN = 16;
#define LED_COUNT 11

uint32_t colours[LED_COUNT] = {0x00ACCC, 0x1D5DC5, 0x541ECB, 0x9C1CEE,
                               0xB30D8B, 0xBC1303, 0xD6AB01, 0xBEFE00,
                               0x69FF03, 0x26FF00, 0x00FF19};

uint32_t led_buffer[LED_COUNT];
uint32_t inBuffer = 0;
void recieve_program_init(PIO pio, uint sm, uint offset, uint pin) {
  pio_sm_config cfg = recieve_program_get_default_config(offset);
  sm_config_set_in_shift(&cfg, false, false, 32);

  sm_config_set_clkdiv(&cfg, (clock_get_hz(clk_sys) / 1e6f) * 2.5f /
                                 8.0f); // 1.25us / 3
  sm_config_set_in_pins(&cfg, pin);

  pio_gpio_init(pio, pin);
  pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);

  pio_sm_init(pio, sm, offset, &cfg);
}

void ws2812a_program_init(PIO pio, uint sm, uint offset, uint pin) {
  pio_sm_config cfg = ws2812a_program_get_default_config(offset);
  sm_config_set_out_shift(&cfg, false, false, 32);
  sm_config_set_clkdiv(&cfg, (clock_get_hz(clk_sys) / 1e6f) * 2.5f /
                                 5.0f); // 1.25us / 3
  sm_config_set_out_pins(&cfg, pin, 1);

  pio_gpio_init(pio, pin);
  pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

  pio_sm_init(pio, sm, offset, &cfg);
}

int main() {
  stdio_init_all();
  PIO outPio = pio0;
  PIO inPio = pio1;
  const uint8_t outSm = 0;
  const uint8_t inSm = 1;
  const uint8_t outDmaIndex = 0;
  const uint8_t inDmaIndex = 1;

  const uint32_t outOffset = pio_add_program(outPio, &ws2812a_program);
  const uint32_t inOffset = pio_add_program(inPio, &recieve_program);
  ws2812a_program_init(outPio, outSm, outOffset, OUTPUT_PIN);
  recieve_program_init(inPio, inSm, inOffset, INPUT_PIN);
  
  // initialise the used GPIO pin to LOW
  gpio_put(OUTPUT_PIN, false);
  gpio_put(OUTPUT_PIN, false);

  // wait for the WS2812s to reset
  sleep_us(50);

  // configure DMA to copy the LED buffer to the PIO state machine's FIFO
  dma_channel_config dma_ch0 = dma_channel_get_default_config(outDmaIndex);
  channel_config_set_transfer_data_size(&dma_ch0, DMA_SIZE_32);
  channel_config_set_read_increment(&dma_ch0, true);
  channel_config_set_write_increment(&dma_ch0, false);
  channel_config_set_dreq(&dma_ch0, DREQ_PIO0_TX0);

  /*dma_channel_config dma_ch1=dma_channel_get_default_config(inDmaIndex);
  channel_config_set_transfer_data_size(&dma_ch1,DMA_SIZE_32);
  channel_config_set_read_increment(&dma_ch1,false);
  channel_config_set_write_increment(&dma_ch1,false);
  channel_config_set_dreq(&dma_ch1,DREQ_PIO1_RX1);
  */
  // run the state machine
  pio_sm_set_enabled(outPio, outSm, true);
  pio_sm_set_enabled(inPio, inSm, true);

#ifdef USE_WS2812D_PIO
  pio_sm_put_blocking(pio, sm, (LED_COUNT - 1));
#endif

  uint32_t first_colour = 0;

  uint32_t sampleBuf = 0;
    uint8_t count = 0;
  while (true) {
#ifdef USE_WS2812D_PIO
    while (pio_interrupt_get(pio, 0))
      ;
#endif

    // bool samplesReady=pio_interrupt_get(inPio, 0);
    // pio_interrupt_clear(inPio,0);
    inBuffer = pio_sm_get_blocking(inPio, inSm);
    // dma_channel_configure(inDmaIndex,&dma_ch1,&inBuffer,&inPio->rxf[inSm],1,true);
    uint8_t bitCount = 24;
    if (inBuffer > 0) {
      if (count < bitCount) {
        const uint8_t mask = 1 << 1;
        bool rBit = true;
        if ((inBuffer & mask)==2) {
          //printf("One recieved\n");

        } else {
          //printf("Zero recieved\n");
          rBit = false;
        }

        sampleBuf = sampleBuf << 1;
        sampleBuf|=rBit;
        count++;
        // printf("In buffer %zu\n",inBuffer);
      } else {
            printf("Sample %zu recieved\n",sampleBuf);
        count=0;
      }
      inBuffer=0;
    }
    // uint32_t rx=inPio->rxf[inSm];
    /*if(rx>0)
    {
        printf("Rx is %zu\n",rx);
    }
    */

    // if(
    for (uint32_t i = 0; i < LED_COUNT; i++) {
      led_buffer[i] = colours[(i + first_colour) % LED_COUNT];
      //led_buffer[i] = colours[0];
    }

    // initiate DMA transfer
    dma_channel_configure(outDmaIndex, &dma_ch0, &outPio->txf[outSm],
                          led_buffer, LED_COUNT, true);

    first_colour = (first_colour + 1) % LED_COUNT;
  }

  return 0;
}
