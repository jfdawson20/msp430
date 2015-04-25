#include "hardware.h"
#include <msp430g2553.h>

volatile struct PIDControl PIDHandle0; 

int main(void) 
{

  HardwareInit();
  PIDInit(&PIDHandle0);
  SetPIDTuning(&PIDHandle0, 2, 1,0);
  PIDSetPoint(&PIDHandle0,150);

  while(1)
  {

  }

  return(0);
}

 
