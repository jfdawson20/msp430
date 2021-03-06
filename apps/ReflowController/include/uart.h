#ifndef UART_H
#define UART_H

#include "hardware.h"

/* UART shaired variables */
volatile unsigned int  rx_flag;			//Mailbox Flag for the rx_char.
volatile unsigned char rx_char;			//This char is the most current char to come out of the UART
volatile unsigned int  tx_flag;			//Mailbox Flag for the rx_char.
volatile unsigned char tx_char;			//This char is the most current char to come out of the UART



/* Function Prototypes */
int  UartInit();
int  UartWriteByte(char c);
char UartReadByte(void);
int  UartReadBuffer(char *s, int count);
int  UartWriteBuffer(char  *s,char nl);
void user_USCI0RX_ISR(void);


#endif //UART_H
