#ifndef RECIEVER_H
#define  RECIEVER_H   
#include <pico/stdlib.h>
#include<hardware/pio.h>
class reciever{
public:
    reciever();
    bool ReadSamples(uint32_t &data);
private:
    PIO inPio = pio1;
    const uint8_t inDmaIndex = 1;
    const uint8_t inSm = 1;

};
#endif
