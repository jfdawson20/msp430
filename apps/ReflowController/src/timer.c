#include "hardware.h"

extern volatile struct PIDControl PIDHandle0; 

/* Initialize timer A0 
   Used to set the PIDcalculate update period */
int TimerA0Init(int ticks)
{          

	TA0CCTL0 = 0b0000000000010000; //capture// CCIFG Enable
	TA0CCR0  = ticks; 
	TA0CTL   = 0b0000000100010000;//ACLK source, divider of 1, Up mode (12 Khz)
	return(0);
}

/* Initialize Timer A0 as a PWM controller 
   IO P2.1 and P2.4 will be the two PDM outputs */
int TimerA1Init()
{
    //setup output ports
    P2SEL   = 0b00010010;  //set p2.1, p2.4 as timer outputs 
    P2SEL2  = 0b00000000;   
    P2DIR   = 0b00010011;
	
    TA1CCR0  = T_1000MS; //set period to 1 second
	TA1CCTL1 = 0b0000000011000000; //compare, CCIFG disable, toggle/set, output defaults high 
	TA1CCTL2 = 0b0000000011000000; //compare, CCIFG disable, toggle/set, output defaults high 
    TA1CCR1  = MINOUT_12k ; //min duty cycle
    TA1CCR2  = MINOUT_12k ; //min duty cycle
	TA1CTL   = 0b0000000100010000;//ACLK source, divider of 1, Up mode (12 Khz)
	return(0);
}

/* Update PWM (i.e. current duty cycle + offset)*/
int PWMUpdate(int duty, int out)
{ 
	unsigned int current;

    /* Grab the current duty cycle */
    if (out == 2)
	{
	 	current = TA1CCR2;
	}
	else if (out == 1) 
	{
		current = TA1CCR1;
	}
	else
	{
		return(-1);
	}

    /* Only update PWM duty cycle if it falls within the minimum and maximum ranges */
	if((duty + current) >= MINOUT_12k && (duty + current) <= MAXOUT_12k)
	{
		if (out == 2) 
			TA1CCR2 = (unsigned int)(duty + current);

		else 
			TA1CCR1 = (unsigned int)(duty + current);

		return(0);
	}
    /* Return -1 on failure */
	else
	{
		return(-1);
	}
	
}

/* Set the current PWM duty cycle */
int PWMSet(unsigned int duty, int out)
{	
	if((duty) >= MINOUT_12k && (duty) <= MAXOUT_12k)
	{
		if (out == 2) 
			TA1CCR2 = (unsigned int)(duty);

		else 
			TA1CCR1 = (unsigned int)(duty);

		return(0);
	}
	else
	{
		return(-1);
	}
	
}


/* Timer A0 interrupt 
   Each interrupt triggers a recalculation of the PID control loop */
#pragma vector = TIMER0_A0_VECTOR 
__interrupt void TA0_ISR (void)
{
	TACCTL0 &= 0xFFFE; //clear interrupt flag 
	CalculatePID(&PIDHandle0);
}

