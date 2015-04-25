#include <io.h>
#include <signal.h>
#include <iomacros.h>
#include <msp430g2553.h>



/* Memory Map */
#define SFR_BASE             0x0000
#define SFR_TOP              0x000F
#define R8_BIT_PERIPH_BASE   0x0010
#define R8_BIT_PERIPH_TOP    0x00FF
#define R16_BIT_PERIPH_BASE  0x100
#define R16_BIT_PERIPH_TOP   0x1FF
#define RAM_BASE             0x0200
#define RAM_TOP              0x03FF
#define INFO_BASE            0x1000
#define INFO_TOP             0x10FF
#define USER_VEC_TABLE_BASE  0xC000
#define USER_VEC_TABLE_TOP   0xC01E
#define USER_FLASH_BASE      0xC020
#define USER_FLASH_TOP       0xF9FF
#define BOOT_FLASH_BASE      0xFBFF
#define BOOT_FLASH_TOP       0xFFDE
#define MSR_VEC_TABLE_BASE   0xFFE0
#define MSR_VEC_TABLE_TOP    0xFFFE

/* other defines */
#define ENTER_BOOT 0xAA

int BootgetC_NonBlocking(char *readVal);
char BootgetC_Blocking();
int  BootputC(char c);
int  UartWriteBuffer(char *s);
int WriteAck(int val);
int eraseFlash();
int writeFlashByte(unsigned char data, unsigned int address);
int writelineFlash(unsigned char * buf, unsigned int address, unsigned int byteCount);
unsigned int strToHex(unsigned char * buf, int start, int end);
unsigned int asciiToHex(char ascii) ;
int calcCB(unsigned char *data, int len, unsigned char expected);
char parseLine(unsigned char * buffer, int length);

int bootloader()
{
 
  unsigned long timeoutCount = 0;
  int enterBoot = -1;
  char read = 0;
  unsigned char readBuffer[64];
  char success = 0;
  unsigned int i = 0;
  unsigned int startAddress = 0;
  unsigned int endAddress = 0;
  
  
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
  while(1)
  {   
      read = BootgetC_Blocking();
      i = 0;
      if (read == 'E') 
      {
            read = BootgetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                readBuffer[i] = read;
                read = BootgetC_Blocking();
                i = i + 1;
            }
            if (readBuffer[0] == 'U')
            {
                i = USER_FLASH_BASE;
                while(i < USER_FLASH_TOP)
                {
                    eraseFlash(i);
                    i = i + 0x200;
                }
                BootputC('+');
            }
            else
            {
                BootputC('-');
            }
            
      }
      
      /* Read Address */
      else if (read == 'R')
      {
            read = BootgetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                readBuffer[i] = read;
                read = BootgetC_Blocking();
                i = i + 1;
            }
            
            if (i == 8) 
            {
                  
                  startAddress = strToHex(readBuffer,0,3);
                  endAddress = strToHex(readBuffer,4,7);
                  for (i = startAddress;i<startAddress+(endAddress-startAddress);i++)
                  {
                        BootputC(*(unsigned char *)i);
                  }
                  //BootputC('+');
                  
            }
            else 
            {
                  BootputC('-');
            }
      }
      else if (read == 'W') 
      {
            read = BootgetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                readBuffer[i] = read;
                read = BootgetC_Blocking();
                i = i + 1;
            }
            
            if (i == 8) 
            {
                  
                  startAddress = strToHex(readBuffer,0,3);
                  endAddress = strToHex(readBuffer,4,7);
                  for (i = startAddress;i<startAddress+(endAddress-startAddress);i++)
                  {
                        BootputC(*(unsigned char *)i);
                  }
                  //BootputC('+');
                  
            }
            else 
            {
                  BootputC('-');
            }
      }
      else if (read == 'X') 
      {
	        UartWriteBuffer("Exit Bootloader\r\n");
	        return(0);
      }

      else if (read == ':') 
      {
            read = BootgetC_Blocking();
            while((read != '\0') && (read != '\r') )
            {
                
                readBuffer[i] = read;
                read = BootgetC_Blocking();
                i = i + 1;
            }
            
            success = parseLine(readBuffer,i);
            BootputC(success);
      }

  }
 
  //jump to user reset vector 
  
  return (0);
}

int BootgetC_NonBlocking(char *readVal)
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

char BootgetC_Blocking()
{
    char rx_ret;
    while ((UC0IFG & 0x0001) == 0x0000);		 		//Wait for rx_flag to be set
    rx_ret = UCA0RXBUF;
    return(rx_ret);
}

int BootputC(char c)
{
    while((UCA0STAT & 0x01) != 0); //block while USCI is busy
    UCA0TXBUF = c;
    return(0);
}

int  UartWriteBuffer(char *s)
{
    int j = 0;
    while(s[j] != '\n')
    {
        BootputC(s[j]);
        j++;
    }
    BootputC('\n');
    BootputC('\r');
    return(0);
}

int eraseFlash(unsigned int dummyWrite)
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

int writeFlashByte(unsigned char data, unsigned int address)
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

int writelineFlash(unsigned char * buf, unsigned int address, unsigned int byteCount)
{
    int ret = 0;
    int i = 0;
    int adcounter = address;
    for(i = 0; i<byteCount*2;i = i + 2)
    {
        ret = writeFlashByte(strToHex(buf,i,i+1),adcounter);
        adcounter++;
        if(ret == -1)
        {
            return(-1);
        }
    }
    
    return(0);
}

char parseLine(unsigned char *workBuffer, int i)
{
        
        unsigned char  byteCount  = 0; 
	    unsigned int   address    = 0;
	    unsigned char  hexID      = 0;
	    unsigned char  checkByte  = 0;
	    unsigned char  cb         = 0;
        
        byteCount  = strToHex(workBuffer,0,1); 
	    address    = strToHex(workBuffer,2,5); 
	    hexID      = strToHex(workBuffer,6,7);
	    checkByte  = strToHex(workBuffer,i-2,i-1);

	    cb = calcCB(workBuffer,i-2 , checkByte);
        
        if (cb == 0 )
        {
            
            /* write program data */
		    if(hexID == 0x00)
		    {
                  writelineFlash((workBuffer+8), address,byteCount);
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

unsigned int strToHex(unsigned char * buf, int start, int end)
{
	unsigned int n = 0;
	int c  = start;
	while(c <= end)
	{
		n = (n << 4) + asciiToHex(buf[c]);
		c = c + 1;
	}
	
	return(n);
}

unsigned int asciiToHex(char ascii) 
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

int calcCB(unsigned char *data, int len, unsigned char expected)
{
	int i;
	unsigned int sum = 0;
	for (i = 0; i< len;i=i+2)
	{
        sum += strToHex(data,i,i+1);
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



