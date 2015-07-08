#include <io.h>
#include <signal.h>
#include <iomacros.h>
#include <msp430g2553.h>
#include "boot.h"

/* Main Bootloader Code */
int bootloader()
{
 
  /* Variable Declarations */
  unsigned long timeoutCount = 0;
  int           enterBoot    = -1;
  char          read         = 0;
  unsigned char readBuffer[64];
  char          success      = 0;
  unsigned int  i            = 0;
  unsigned int  startAddress = 0;
  unsigned int  endAddress   = 0;
  
  
  /********************************** System HW Init *********************************/
  WDTCTL  = WDTPW + WDTHOLD;   //Disable WDT 
  
  //Use macros to set DCO output to 16 MHz 
  DCOCTL  = 0;
  BCSCTL1 = CALBC1_16MHZ & 0b11001111;  //set to 16 MHz cal & set ACLK DIV to 0
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL2 = 0b00000100; //SMCLK Divisor set to 4 (4 Mhz) 
  BCSCTL3 = 0b00100000; //select VLCLK (12 kHz) as ACLK source 
  
  /********************************** Uart putC(*(unsigned char *)i);Init *********************************/ 
  /* Setup I/O ports */
  P1SEL   = 0b00000110;   // Set P1.1 & P1.2 mux to use UART pins
  P1SEL2  = 0b00010110;   // Set P1.1 & P1.2 mux to use UART pins
  P1DIR   = 0b00000001;   // Set p1.2 (UART-TX) As Output, p1.1 as input, p1.0 as output (LED)
  P1OUT   = 0b00000000;   // reset P1.2
    
  /* uart config */
  UCA0CTL1 = 0b00000001;  //Hold USCI-Uart in reset
  UCA0CTL0 = 0b00000000;  //setup uart configs
  UCA0CTL1 = 0b10000001;  //set SMCLOCK as clock source
  UCA0BR0  = 2;           //baud rate control upper byte?
  UCA0BR1  = 0;           //baud rate control lower  byte?
  UCA0MCTL = 0b0010111;
  //UCA0BR0  = 26;           //baud rate control upper byte?
  //UCA0BR1  = 0;           //baud rate control lower  byte?
  //UCA0MCTL = 0b0001001;
    
  /* Dis-assert Reset */
  UCA0CTL1 &= ~UCSWRST;  
  
  /********************************** Flash Controller init *********************************/
  
  UartWriteBuffer("clearc\r\n");
  
  /* main loop */
  while(1)
  {   
      //Wait for incoming character from host
      read = BootGetC_Blocking();
      i = 0;
      // Erase command received 
      // Erase User Flash command : ascii 'EU'
      if (read == 'E') 
      {
            read = BootGetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                readBuffer[i] = read;
                read = BootGetC_Blocking();
                i = i + 1;
            }
            if (readBuffer[0] == 'U')
            {
                i = USER_FLASH_BASE;
                while(i < USER_FLASH_TOP)
                {
                    EraseFlash(i);
                    i = i + 0x200;
                }
                //Return ascii '+' on sucessfu
                BootPutC('+');
            }
            else
            {
                //return ascii '-' on failure
                BootPutC('-');
            }
            
      }
      
      /* Read Address */
      else if (read == 'R')
      {
            read = BootGetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                readBuffer[i] = read;
                read = BootGetC_Blocking();
                i = i + 1;
            }
            
            // Check if command recieved is the correct length 
            // A vaild read command needs eight characters 
            // 0 to 3 is the start address in hex 
            // 4 to 7 is the end address in hex 
            if (i == 8) 
            {
                  
                  startAddress = StrToHex(readBuffer,0,3);
                  endAddress = StrToHex(readBuffer,4,7);
                  for (i = startAddress;i<startAddress+(endAddress-startAddress);i++)
                  {
                        BootPutC(*(unsigned char *)i);
                  }
   
            }
            else 
            {
                  BootPutC('-');
            }
      }
    
      // receiving a single 'X' will force the bootloader
      // to exit. See '.global reset' function in reset.asm 
      // for continued program flow 
      else if (read == 'X') 
      {
	        UartWriteBuffer("Exit Bootloader\r\n");
	        return(0);
      }

      // start of hex file line 
      else if (read == ':') 
      {
            read = BootGetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                
                readBuffer[i] = read;
                read = BootGetC_Blocking();
                i = i + 1;
            }
            
            //parse full line and return result '+' or '-'
            success = ParseLine(readBuffer,i);
            BootPutC(success);
      }

  }
 
  //jump to user reset vector 
  
  return (0);
}

/* Non-blocking Uart Rx Receive function 
   checks Rx recieve flag and if false returns.
   if true it sets the readVal pointer and returns 0 */
   
int BootGetC_NonBlocking(char *readVal)
{
    int flag = -1;
    if ((UC0IFG & 0x0001) == 0x0001)
    {
       *readVal = UCA0RXBUF;
       flag = 0;
    }
    else

    {
	    *readVal = -1;
	    flag = -1;
    }
    return(flag);
}

/* Blocking UART Rx Receive 
   Waits indefinitely for UART to receive a byte */
char BootGetC_Blocking()
{
    char rx_ret;
    while ((UC0IFG & 0x0001) == 0x0000);		 		//Wait for rx_flag to be set
    rx_ret = UCA0RXBUF;
    return(rx_ret);
}

/* UART Tx Character */
int BootPutC(char c)
{
    while((UCA0STAT & 0x01) != 0); //block while USCI is busy
    UCA0TXBUF = c;
    return(0);
}

/* UART Tx Buffer */

int UartWriteBuffer(char *s)
{
    int j = 0;
    while(s[j] != '\n')
    {
        BootPutC(s[j]);
        j++;
    }
    BootPutC('\n');
    BootPutC('\r');
    return(0);
}

/* Erase Flash Function 
   Used to erase user flash memory */
int EraseFlash(unsigned int dummyWrite)
{
    //make sure sector to erase is valid!!!
    if ((dummyWrite > BOOT_FLASH_BASE) || (dummyWrite < USER_VEC_TABLE_BASE))
    {
      return(-1); 
    }
    
    while((FCTL3 & 0x0001) == 0x0001); //wait for not busy 
    
    FCTL2 = ((0xA500) | (0x0084)); //Password (0xA5), SMCLK source(0x80), divisor of 5 (4+1, 0x04) 
    FCTL3 = ((0xA500) & (0xFF00)); //Password (0xA5), Clear lock   
    FCTL1 = ((0xA500) | (0x0002)); //Password (0xA5), enable erase 
    
    *((unsigned char *)dummyWrite) = 0x0000; //dummy write to trigger erase 
    
    while((FCTL3 & 0x0001) == 0x0001); //wait for not busy 
    FCTL1 = ((0xA500) & (0xFF00)); //Password (0xA5), clear erase
    FCTL3 = ((0xA500) | (0x0010)); //Password (0xA5), set lock  
    
    return(0);
}

/* Write byte to flash function */
int WriteFlashByte(unsigned char data, unsigned int address)
{
    
    //make sure address to write is valid!!!
    if ((address > BOOT_FLASH_BASE) || (address < USER_VEC_TABLE_BASE))
    {
      return(-1); 
    }

    while((FCTL3 & 0x0001) == 0x0001); //wait for not busy 
    
    FCTL2 = ((0xA500) | (0x0084)); //Password (0xA5), SMCLK source(0x80), divisor of 5 (4+1, 0x04) 
    FCTL3 = ((0xA500) & (0xFF00)); //Password (0xA5), Clear lock  
    FCTL1 = ((0xA500) | (0x0040)); //Password (0xA5), enable byte write
    
    *((unsigned char *)address) = data ; //dummy write to trigger erase 
    
    while((FCTL3 & 0x0001) == 0x0001); //wait for not busy
    FCTL1 = ((0xA500) | (0x0000)); //Password (0xA5), clear write 
    FCTL3 = ((0xA500) | (0x0010)); //Password (0xA5), set lock  
     
    return(0);
}

/* Write Hex file line to flash */
int WriteLineFlash(unsigned char * buf, unsigned int address, unsigned int byteCount)
{
    int ret = 0;
    int i = 0;
    int adcounter = address;
    for(i = 0; i<byteCount*2;i = i + 2)
    {
        ret = WriteFlashByte(StrToHex(buf,i,i+1),adcounter);
        adcounter++;
        if(ret == -1)
        {
            return(-1);
        }
    }
    
    return(0);
}

/* Parse incoming hex file line */
char ParseLine(unsigned char *workBuffer, int i)
{
        
        unsigned char  byteCount  = 0; 
	    unsigned int   address    = 0;
	    unsigned char  hexID      = 0;
	    unsigned char  checkByte  = 0;
	    unsigned char  cb         = 0;
        
        byteCount  = StrToHex(workBuffer,0,1); 
	    address    = StrToHex(workBuffer,2,5); 
	    hexID      = StrToHex(workBuffer,6,7);
	    checkByte  = StrToHex(workBuffer,i-2,i-1);

	    cb = CalcCB(workBuffer,i-2 , checkByte);
        
        if (cb == 0 )
        {
            
            /* write program data */
		    if(hexID == 0x00)
		    {
                  WriteLineFlash((workBuffer+8), address,byteCount);
		    }
		
		    /* End of File */
		    else if (hexID == 0x01) 
		    {

		    }
				
		    /* extended segment address , no used/supported */
		    else if (hexID == 0x02) 
		    {

		    }
				        
		    /* Start Segment Address, sets instruction pointer to start of code base */
		    else if (hexID == 0x03) 
		    {

		    }
				
		    /* Extended linear address, 32 bit addressing, not used/supported */
		    else if (hexID == 0x04) 
		    {
               
		    }
				
		    /* start linear address, not supported/used */
		    else if (hexID == 0x05)
		    {
				
		    }
				
		    /* invalid hex ID */
		    else
		    {
			
		    }
		}
		else
		{
		    return('-');
		}
	return('+');
}

/* Convert subsection of a c string into a a hex integer */
unsigned int StrToHex(unsigned char * buf, int start, int end)
{
	unsigned int n = 0;
	int c  = start;
	while(c <= end)
	{
		n = (n << 4) + AsciiToHex(buf[c]);
		c = c + 1;
	}
	
	return(n);
}

/* Convert ascii character to its hex representation 
   e.g. 'A' converts to 0xA */
unsigned int AsciiToHex(char ascii) 
{

    int i = 0; 
    char Dict[16];   
    Dict[0]  = '0';
	Dict[1]  = '1';
	Dict[2]  = '2';
	Dict[3]  = '3';
	Dict[4]  = '4';
	Dict[5]  = '5';
	Dict[6]  = '6';
	Dict[7]  = '7';
	Dict[8]  = '8';
	Dict[9]  = '9';
	Dict[10] = 'A';
	Dict[11] = 'B';
	Dict[12] = 'C';
	Dict[13] = 'D';
	Dict[14] = 'E';
	Dict[15] = 'F';
	
	for (i = 0; i < 16;i++) 
	{
		if (Dict[i] == ascii)
		{
			return(i);
		}
	}

	return (-1);
}

/* Calculate checkbyte and compare to expected checkbyte value */
int CalcCB(unsigned char *data, int len, unsigned char expected)
{
	int i;
	unsigned int sum = 0;
	for (i = 0; i< len;i=i+2)
	{
        sum += StrToHex(data,i,i+1);
	}
	
	sum = (~sum) + 1; 
	if ((unsigned char)sum != expected) 
	{
        return(-1);
	}
    else
    {
        return(0);
    }
}



