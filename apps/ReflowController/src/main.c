#include "hardware.h"
#include <msp430g2553.h>

volatile struct PIDControl PIDHandle0; 

int main(void) 
{

  /* Initalize HW: Peripherals, Clocks, WDT*/
  HardwareInit();
  
  /* Create a PID controller struct */
  PIDInit(&PIDHandle0);
  
  /* set P, I and D coefficients */
  SetPIDTuning(&PIDHandle0, 2, 1,0);
  
  /* Set initial temperature target to 150C*/
  PIDSetPoint(&PIDHandle0,150);

  while(1){}

  return(0);
}

 
