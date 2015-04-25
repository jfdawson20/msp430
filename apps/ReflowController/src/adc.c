#include "hardware.h"

int ADCInit()
{
    /* Set ADC: 
       Rate       : 25 KS/s
       Inputs     : A0, A3, A4, A5 
       Mode       : Single 
       Interrupt  : Set on completion 
       DTC        : Disabled 
    */
    
    ADC10CTL0 = 0x0000;             // Disable conversions 
    ADC10CTL0 = 0b0001100000011000; // ADC10 On, Reference Gen Off, MSC set
    ADC10CTL1 = 0b0000000011111000; // SMCLK source, ADC10 Clock Divisor = 8
    ADC10AE0  = 0b00000000;         // Enable analog inputs A5,4,3,0
    
    A0 = 0;
    A3 = 0;
    A4 = 0;
    A5 = 0;
    currentChannel = 0;
    SeqDone = 0;
    ADC10CTL1 &= 0x0FFF; //set channel number 
    ADC10CTL0 |= 0x0003; //start sampling 
    
    return(0);
}


unsigned int ADCReadChannel(char chanNum)
{
    while(SeqDone == 0);

    if (chanNum == 0)
    {
      return(A0);
    }
    
    else if (chanNum == 3)
    {
      return(A3);
    }
    
    else if (chanNum == 4) 
    {
      return(A4);
    }
    else if (chanNum == 5) 
    {
      return(A5);
    }

    else 
    {
      return (0xFFFF);
    }
}

int ADCReadTemp(char chanNum)
{
    uint32_t temp = 0;
	temp = (ADCReadChannel(chanNum)*10)/32 ;
	return((int)temp);
}

int ADCStart(char chanNum)
{
  if (chanNum == 0)
  {
    ADC10CTL1 &= 0x0FFF; //set channel number 
    ADC10CTL0 |= 0x0003; //start sampling 
  }
  
  else if (chanNum == 3) 
  {
	ADC10CTL1 &= 0x0FFF; //set channel number 
    ADC10CTL1 |= 0x3000; //set channel number 
    ADC10CTL0 |= 0x0003; //start sampling 
  }
  
  else if (chanNum  == 4) 
  {
	ADC10CTL1 &= 0x0FFF; //set channel number 
    ADC10CTL1 |= 0x4000; //set channel number
    ADC10CTL0 |= 0x0003; //start sampling 
  }

  else if (chanNum == 5) 
  {
	ADC10CTL1 &= 0x0FFF; //set channel number 
    ADC10CTL1 |= 0x5000; //set channel number 
    ADC10CTL0 |= 0x0003; //start sampling    
  }
  else 
  {
    return (-1); 
  }  
     
} 

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void) 
{

  
  if (currentChannel == 0)
  {
    A0 = ADC10MEM;
    currentChannel = 3;
  }

  else if (currentChannel == 3)
  {
    A3 = ADC10MEM;
    currentChannel = 4;
  }
  else if (currentChannel == 4)
  {
    A4 = ADC10MEM;
    currentChannel = 5;
  }
  else if (currentChannel == 5) 
  {
    A5 = ADC10MEM;
    currentChannel = 0;
    SeqDone = 1;
  }
 
   ADC10CTL0 &= 0xFFFC;		
   ADC10CTL0 &= ~ADC10IFG;  // clear interrupt flag
   ADCStart(currentChannel);
}
