#include "hardware/structs/clocks.h"
#include"reciever.h"
#include<stdio.h>
#include<cstdlib>
#include<time.h>
#include<iostream>
bool processSamples(uint32_t samples)
{
  const uint8_t recoverMask=1<<1;
  return samples&recoverMask; 
}
int main()
{
  stdio_init_all();
  reciever rec;
  uint32_t samples=0;
  sleep_us(50);
  //clock_t currSampleT=0;
  //clock_t lastSampleT=time_us_64();
    uint8_t bitCount=0;
  uint32_t packet=0;
  while(true)
  {
    
   if(rec.SamplesReady())   
    {
      std::cout<<"Samples ready \n";
      samples=rec.GetSamples();
      //currSampleT=time_us_64();
      //clock_t diff=currSampleT-lastSampleT;
      
      //lastSampleT=currSampleT;
      /*if(diff>50)
      {
        packet=0;
        bitCount=0;
      }

        std::cout<<"Packet reset diff "<<diff<<"\n";
      */
      //std::cout<<"us since last sample "<<diff<<" \n";

      if(bitCount>=24)
      {
        printf("Packet recieved %zu\n",packet);
        packet=0;
        bitCount=0;
      }
    bool recoverBit=processSamples(samples);
    printf("samples %zu recovered bit %d \n",samples,recoverBit);
    
        packet<<=1; 
        packet|=recoverBit;
      bitCount++;
    }
  }
  return EXIT_SUCCESS;
}
