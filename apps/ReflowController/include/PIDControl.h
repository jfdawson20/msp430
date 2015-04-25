#ifndef PIDCONTROL_H
#define PIDCONTROL_H

#include "hardware.h"

struct PIDControl 
{
  int coefP;
  int coefI;
  int coefD; 
  int setPoint;
  int sum; 
  int lastTime; 
};

int PIDInit(volatile struct PIDControl *p);
int SetPIDTuning(volatile struct PIDControl *p, int P, int I, int D);
int PIDSetPoint(volatile struct PIDControl *p, int setpoint);
int CalculatePID(volatile struct PIDControl *p);
int AutoCal(volatile struct PIDControl *p);






#endif
