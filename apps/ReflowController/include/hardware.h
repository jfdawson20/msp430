#ifndef HARDWARE_H
#define HARDWARE_H


#include <msp430g2553.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "misc.h"
#include "timer.h"
#include "PIDControl.h"
#include "stringFunctions.h"
#include "adc.h"


/* Timer Defines */
#define T_1000MS 12048
#define T_100MS  1204
#define T_10MS   120
#define T_1MS    12

//PWM Constants 
//Assuming 15 ms minimum switching time for SSRs
//assuming 1s period
#define MINOUT_12k 200
#define MAXOUT_12k 11848 

//temperature constants 
#define ILimit 30
#endif //HARDWARE_H

//PID Handle 
