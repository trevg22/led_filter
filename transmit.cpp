#include "transmitter.h"
#include<cstdlib>

#define LED_COUNT 11
uint32_t colours[LED_COUNT] = 
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

int main(){

transmitter trans;
sleep_us(150);
while(true)
    {
        uint32_t first_colour = colours[1];
    first_colour =3;
        trans.transmit(first_colour);
        sleep_us(200);
    }
return EXIT_SUCCESS ;
}
