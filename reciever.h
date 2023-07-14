#ifndef RECIEVER_H
#define  RECIEVER_H   
#include <pico/stdlib.h>
#include<hardware/pio.h>
#include<hardware/dma.h>

class reciever{
public:
    reciever();
    uint32_t GetSamples();
    bool SamplesReady();

private:
    PIO inPio = pio1;
    uint32_t data=0;
    const uint8_t inDmaIndex = 1;
    const uint8_t inSm = 1;

    dma_channel_config dma_ch1=dma_channel_get_default_config(inDmaIndex);


};
#endif
