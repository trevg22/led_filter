#include"pico/stdlib.h"
#include"hardware/pio.h"
#include"hardware/dma.h"
#include"hardware/clocks.h"
#include"rx.pio.h"
#include<cstdlib>
#include<iostream>
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
int main()
{
  stdio_init_all();
  uint32_t buffer=0;
  PIO pio=pio1;
  uint8_t sm=1;
  uint32_t offset= pio_add_program(pio,&recieve_program);
  recieve_program_init(pio,sm,offset,16);
int8_t dma_chan = dma_claim_unused_channel(true);
dma_channel_config c = dma_channel_get_default_config(dma_chan);
channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
channel_config_set_read_increment(&c, false);
channel_config_set_dreq(&c, DREQ_PIO0_RX0);
dma_channel_configure(
dma_chan,
&c,&buffer, // Write address (only need to set this once)
&pio1_hw->rxf[0],
 // Don't provide a read address yet
1, // Write the same value many times, then halt and interrupt
false
 // Don't start yet
);

  dma_channel_set_irq0_enabled(dma_chan,true);
  irq_set_enabled(DMA_IRQ_0,true);
   printf("Program running\n"); 
  while(true){
  }
  return EXIT_SUCCESS;
}
