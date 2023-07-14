#include "rx.pio.h"
#include"reciever.h"
#include"hardware/clocks.h"

const uint8_t INPUT_PIN = 16;
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

reciever::reciever(){
const uint32_t inOffset = pio_add_program(inPio, &recieve_program);
recieve_program_init(inPio, inSm, inOffset, INPUT_PIN);
gpio_put(INPUT_PIN, false);
pio_sm_set_enabled(inPio, inSm, true);

channel_config_set_transfer_data_size(&dma_ch1,DMA_SIZE_32);
  channel_config_set_read_increment(&dma_ch1,false);
  channel_config_set_write_increment(&dma_ch1,false);
  channel_config_set_dreq(&dma_ch1,DREQ_PIO1_RX0);
  dma_channel_configure(inDmaIndex,&dma_ch1,&data,&inPio->rxf[inSm],1,false); 
  dma_channel_set_irq0_enabled(inDmaIndex, true);
  irq_set_enabled(DMA_IRQ_0,true);
  dma_channel_start(inDmaIndex);
}

bool reciever::SamplesReady()
{
  return dma_channel_get_irq0_status(inDmaIndex);
}
uint32_t reciever::GetSamples()
{
   //dma_channel_acknowledge_irq0(inDmaIndex); 
    //data= pio_sm_get_blocking(inPio, inSm);
    //pio_sm_clear_fifos(inPio,inSm);
    return data;
}
