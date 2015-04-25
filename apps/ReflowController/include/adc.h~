#ifndef ADC_H
#define ADC_H

#include "hardware.h"

volatile unsigned int A0, A3, A4, A5;
volatile char  currentChannel; 
volatile char  SeqDone; 

int ADCInit();
int ADCStart(char chanNum);
unsigned int ADCReadChannel(char chanNum);
int ADCReadTemp(char chanNum);


#endif //adc_H
