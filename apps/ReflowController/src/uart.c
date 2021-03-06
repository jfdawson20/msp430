#include "hardware.h"

/*  Function       : UartInit()
	Author         : Jack Dawson
	Date           : 11/26/2014
	Inputs         : None
	Description    : Initializes UART core.
					 115200 kBAUD
					 8 data, 1 stop, no parity
					 Return Values  : int - 0 (success)
*/

/* Global buffers and stuff */
char UWriteBuffer[64] = {};
char UReadBuffer[64] = {};
unsigned int recCounter = 0;


int  UartInit()
{
    //int j = 0;
    /* Setup I/O ports */
    P1SEL   = 0b00000110;   // Set P1.1 & P1.2 mux to use UART pins
    P1SEL2  = 0b00000110;   // Set P1.1 & P1.2 mux to use UART pins
    P1DIR   = 0b00000001;   // Set p1.2 (UART-TX) As Output, p1.1 as input, p1.0 as output (LED)
    P1OUT   = 0b00000000;   // reset P1.2
    
    /* uart config */
    UCA0CTL1 = 0b00000001;  //Hold USCI-Uart in reset
    UCA0CTL0 = 0b00000000;  //setup uart configs
    UCA0CTL1 = 0b10000001;  //set SMCLOCK as clock source
    UCA0BR0  = 2;           //baud rate control upper byte?
    UCA0BR1  = 0;           //baud rate control lower  byte?
    UCA0MCTL = 0b0010111;
    
    /* Dis-assert Reset */
    UCA0CTL1 &= ~UCSWRST;
    
    UC0IE = 0b00000001;    // Enable USCI_A0 RX interrupt
    rx_flag = 0;		   //Set rx_flag to 0
    
    //clear buffers and counters
    memset(UWriteBuffer, 0, 32);
    memset(UReadBuffer, 0, 32);
    recCounter = 0;
    return(0);
}

/* Function       : UartWriteByte(char C)
Author         : Jack Dawson
Date           : 11/26/2014
Inputs         : byte/char to write out
Description    : Write one byte out of UART
Return Values  : int - 0 (success)
*/

int  UartWriteByte(char c)
{
    while((UCA0STAT & 0x01) != 0); //block while USCI is busy
    UCA0TXBUF = c;
    return(0);
}

/*  Function       : UartReadByte(char C)
	Author         : Jack Dawson
	Date           : 11/26/2014
	Inputs         : None
	Description    : Read one byte out of UART, waits for interrupt routine to
					 set mailbox flag. Currently no timeout
	Return Values  : int - 0 (success)
*/

char UartReadByte(void)
{
    char rx_ret;
    while ((UC0IFG & 0x0001) == 0x0000);		 		//Wait for rx_flag to be set
    rx_ret = UCA0RXBUF;
    return(rx_ret);
}

/*  Function       : UartReadBuffer(char C)
	Author         : Jack Dawson
	Date           : 11/26/2014
	Inputs         : buffer s of size count
	Description    : Read buffer s from UART
	Return Values  : int : 0 (success)
*/

int UartReadBuffer(char * s, int count)
{
    int i =0;
    for (i=0; i<count;i++)
        s[i] = UartReadByte();
    
    return(0);
}

/*  Function       : UartWriteBuffer(char C,char nl)
	Author         : Jack Dawson
	Date           : 11/26/2014
	Inputs         : buffer s - of data to write out, nl - string termination style 
	Description    : Write buffer s out of UART
	Return Values  : int : 0 (success)
*/

int  UartWriteBuffer(char *s,char nl)
{
    int j = 0;
    const char begin[] = "\r\n";
    const char end[]   = "\r\n% ";
    char messageToSend[70];
    memset(messageToSend, 0, 70);
    
    if (nl == 0)
    {
        strcpy(messageToSend,begin);
        strcat(messageToSend,s);
        strcat(messageToSend,end);
    }
    else if (nl == 1)
    {
        strcpy(messageToSend,end);
    }
    else if (nl == 2)
    {
        strcpy(messageToSend,s);
    }
    else if (nl ==3)
    {
        strcpy(messageToSend,begin);
        strcat(messageToSend,s);
    }
    while(j < strlen(messageToSend))
    {
        UartWriteByte(messageToSend[j]);
        j++;
    }
    
    return(0);
}

/* Uart Interupt Routines */

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	
    /* Read RX Receive Reg, (Clears Int) */
    char CharRead = UCA0RXBUF;
    int  chanRead = 0;
    int  readCounter = 0;
    unsigned int ADCVal =0;
    int temperature = -1;
    char PWMBuf[10];
    unsigned int PWM;
    

    /* clear tx buffer */
    memset(UWriteBuffer, 0, 64);
        
    /* Process Received Char */
    /* If a return character is received*/
    if (CharRead == 0x0D)
    {
        /* Hi Message: Display software version info */
        if (strcmp(UReadBuffer,"hi") == 0)
        {
            UartWriteBuffer("<----------------------->",3);
            UartWriteBuffer("->Reflow Oven Controller",3);
            UartWriteBuffer("->Version: 1.0",3);
            UartWriteBuffer("->Author: J.Dawson",3);
            UartWriteBuffer("->Type Help For Options",3);
            UartWriteBuffer("<----------------------->",0);
            
        }
        
		/* help message: display list of commands */
        else if(strcmp(UReadBuffer,"help") == 0 )
        {
            UartWriteBuffer("<------ Help Menu ------>",3);
            UartWriteBuffer("tn, read temperature channel n",3);
            UartWriteBuffer("rn, read ADC channel n (raw)",3);
            UartWriteBuffer("un, increment PWM channel n",3);
            UartWriteBuffer("ln, decrement PWM channel n",3);
            UartWriteBuffer("sn x, set PWM channel n to value x",3);
            UartWriteBuffer("fn, Set Fan Output to n",3);
            UartWriteBuffer("<------ Help Menu ------>",0);
            
        }
        
		/* read command: process temperature */
        else if((strstr(UReadBuffer,"t") != NULL) && (strlen(UReadBuffer) == 2) )
        {
			/* channel 0 */
            if ((UReadBuffer[1] - 0x30) == 0)
            {
                chanRead = 0;
                temperature = ADCReadTemp(chanRead);
                //itoa(temperature,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",temperature);
                UartWriteBuffer(UWriteBuffer,0);
                
            }
			
			/* channel 3 */
            else if ((UReadBuffer[1] - 0x30) == 3)
            {
                chanRead = 3;
                temperature = ADCReadTemp(chanRead);
                //itoa(temperature,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",temperature);
                UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* channel 4 */
            else if ((UReadBuffer[1] - 0x30) == 4)
            {
                chanRead = 4;
                temperature = ADCReadTemp(chanRead);
                //itoa(temperature,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",temperature);
                UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* channel 5 */
            else if ((UReadBuffer[1] - 0x30) == 5)
            {
                chanRead = 5;
                temperature = ADCReadTemp(chanRead);
                //itoa(temperature,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",temperature);
                UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* invalid channel number */
            else
            {
                UartWriteBuffer("Channel Not Available",0);
            }
            
        }

		/* read command: process temperature */
        else if((strstr(UReadBuffer,"r") != NULL) && (strlen(UReadBuffer) == 2) )
        {
			/* channel 0 */
            if ((UReadBuffer[1] - 0x30) == 0)
            {
                chanRead = 0;
                ADCVal = ADCReadChannel(chanRead);
                //itoa(ADCVal,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",ADCVal);
                UartWriteBuffer(UWriteBuffer,0);
                
            }
			
			/* channel 3 */
            else if ((UReadBuffer[1] - 0x30) == 3)
            {
                chanRead = 3;
                ADCVal = ADCReadChannel(chanRead);
                //itoa(ADCVal,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",ADCVal);
                UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* channel 4 */
            else if ((UReadBuffer[1] - 0x30) == 4)
            {
                chanRead = 4;
                ADCVal = ADCReadChannel(chanRead);
                //itoa(ADCVal,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",ADCVal);                
								UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* channel 5 */
            else if ((UReadBuffer[1] - 0x30) == 5)
            {
                chanRead = 5;
                ADCVal = ADCReadChannel(chanRead);
                //itoa(ADCVal,UWriteBuffer,10);
								sprintf(UWriteBuffer,"%03u",ADCVal);
                UartWriteBuffer(UWriteBuffer,0);
            }
            
			/* invalid channel number */
            else
            {
                UartWriteBuffer("Channel Not Available",0);
            }
            
        }
        
		/* PWM Increase: Increase PWM duty cycles */
        else if((strstr(UReadBuffer,"u") != NULL) && (strlen(UReadBuffer) == 2) )
        {
			/* PWM Channel 1, P2.1 */
            if ((UReadBuffer[1] - 0x30) == 1)
            {
                PWMUpdate(200, 1);
                UartWriteBuffer("",1);
            }

			/* PWM Channel 2, p2.4 */
            else if ((UReadBuffer[1] - 0x30) == 2)
            {
                PWMUpdate(200, 2);
                UartWriteBuffer("",1);
            }

			/* Invalid PWM Channel */
            else
            {
                UartWriteBuffer("PWM Channel Not Available",0);
            }
        }
        
		/* PWM Decrease: Decrease PWM duty cycles */
        else if((strstr(UReadBuffer,"l") != NULL) && (strlen(UReadBuffer) == 2))
        {
	
			/* PWM Channel 1, P2.1 */
            if ((UReadBuffer[1] - 0x30) == 1)
            {
                PWMUpdate(-200, 1);
                UartWriteBuffer("",1);
            }

			/* PWM Channel 2, p2.4 */
            else if ((UReadBuffer[1] - 0x30) == 2)
            {
                PWMUpdate(-200, 2);
                UartWriteBuffer("",1);
            }

			/* Invalid PWM Channel */
            else
            {
                UartWriteBuffer("PWM Channel Not Available",0);
            }
        }

		/* PWM Set: Set PWM duty cycles */
        else if((strstr(UReadBuffer,"s") != NULL))
        {
			if(strlen(UReadBuffer) > 8)
			{
				UartWriteBuffer("Invalid PWM Value",0);
			}
			
			else
			{
				strcpy(PWMBuf,(UReadBuffer+3));
				PWM = (unsigned int)atoi(PWMBuf);
			
				/* PWM Channel 1, P2.1 */
		        if ((UReadBuffer[1] - 0x30) == 1)
		        {
		            PWMSet(PWM, 1);
		            UartWriteBuffer("",1);
		        }

				/* PWM Channel 2, p2.4 */
		        else if ((UReadBuffer[1] - 0x30) == 2)
		        {
		            PWMSet(PWM, 2);
		            UartWriteBuffer("",1);
		        }

				/* Invalid PWM Channel */
		        else
		        {
		            UartWriteBuffer("PWM Channel Not Available",0);
		        }
			}
        }
        
		/* Fan Control (P2.0): Inable or Disable fan */
        else if((strstr(UReadBuffer,"f") != NULL) && (strlen(UReadBuffer) == 2) )
        {
			/* disable fan */
            if ((UReadBuffer[1] - 0x30) == 0)
            {
				P2OUT &= 0b11111110;
				UartWriteBuffer("",1);
            }
            
			/* enable fan */
            else if ((UReadBuffer[1] - 0x30) == 1)
            {
                P2OUT |= 0b00000001;
				UartWriteBuffer("",1);
            }
            
			/* Invalid Fan Setting */
            else
            {
                UartWriteBuffer("Invalid Fan Setting",0);
            }
        }
        
		/* Catch any unknown commands */
        else if(strlen(UReadBuffer) > 0)
        {
            UartWriteBuffer("Unknown Command",0);
        }
        
		/* print '%' and new line */
        else
        {
            UartWriteBuffer("",1);
        }
        
        memset(UReadBuffer, 0, 64);
        recCounter = 0;
    }

	/* Handle backspaces, update display */
    else if (CharRead == 0x08) 
    {
        if(recCounter > 0 )
        {
            recCounter = recCounter - 1;
            UReadBuffer[recCounter] = '\0';
            UWriteBuffer[0] = 0x08; //backspace
            UWriteBuffer[1] = 0x20; //space bar
            UWriteBuffer[2] = 0x08; //backspace
            UWriteBuffer[3] = '\0';
            UartWriteBuffer(UWriteBuffer,2);
        }
    }

	/* Echo received character */
    else 
    {
        UReadBuffer[recCounter] = CharRead;
        UWriteBuffer[0] = CharRead;
        UWriteBuffer[1] = '\0';
        recCounter = recCounter + 1;
        UartWriteBuffer(UWriteBuffer,2);
    }
    

}



