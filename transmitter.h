#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include"pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

class transmitter {

public:
    transmitter();
    void transmit(uint32_t buffer);
private:
PIO outPio = pio0;
    PIO inPio =pio1;
    const uint8_t outSm = 0;
    const uint8_t outDmaIndex = 0;

uint32_t buffer=0;
dma_channel_config dma_ch0;

};
#endif
