#include "hardware.h"

int HardwareInit()
{
  SysInit();  //setup clocking 
  UartInit(); //init uart 
  ADCInit();  //setup adc 
  TimerA0Init(T_10MS); //init timer A, 10 ms count  
  TimerA1Init(); //setup PWM timers
  __bis_SR_register(GIE); //register global interupts 

  return(0);
}

int SysInit()
{
  //Disable WDT
  WDTCTL  = WDTPW + WDTHOLD; 
  
  //Use macros to set DCO output to 16 MHz 
  DCOCTL  = 0;
  BCSCTL1 = CALBC1_16MHZ & 0b11001111;  //set to 16 MHz cal & set ACLK DIV to 0
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL2 = 0b00000100; //SMCLK Divisor set to 4 (4 Mhz) 
  BCSCTL3 = 0b00100000; //select VLCLK (12 kHz) as ACLK source 

  return(0);
}


/* Delay function */
void delay(unsigned int d) 
{
   int i;
   for (i = 0; i<d; i++) 
   {
      nop();
      nop();
   }
}



