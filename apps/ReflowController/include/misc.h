#ifndef MISC_H
#define MISC_H

#include "hardware.h"

int  HardwareInit();
int  SysInit();
void delay(unsigned int d);
int UartTest();
int ADCTest(); 

#ifdef DEBUG
int ftoa (char * buf, float z, int prec);
#endif 

#endif //MISC_H
