#include "hardware.h"


int PIDInit(volatile struct PIDControl *p)
{
  p->coefP = 3;
  p->coefI = 0;
  p->coefD = 0;
  p->setPoint  = 0; 
  p->sum       = 0;
  p->lastTime  = 0;
   
  return(0);
}
int SetPIDTuning(volatile struct PIDControl *p, int P, int I, int D)
{
  p->coefP = P;
  p->coefI = I; 
  p->coefD = D;
  return(0);
}

int CalculatePID(volatile struct PIDControl *pstruct)
{
  int32_t u;
  int16_t error; 
  uint16_t temp;
  int16_t p;
  int16_t i;
  int16_t d;   
  temp  =  ADCReadTemp(3);
  
  //calculate error 
  error = pstruct->setPoint - temp;
  
  //calculate P term
  p = (pstruct->coefP) * error; 
  
  //calculate I term
  pstruct->sum = pstruct->sum + error; 
  if(pstruct->sum > ILimit) 
  {
	pstruct->sum = ILimit; 
  }
  
  else if(pstruct->sum < -ILimit) 
  {
	pstruct->sum = -ILimit;
  }
  
  i = (pstruct->coefI) * (pstruct->sum);
  
  //calculate D term 
  d = (pstruct->coefD) * (error - pstruct->lastTime);
  pstruct->lastTime = error; //update n-1 stored error 
  
  u = p + i + d;
  
  //ensure u is a valid PWM value 
  if(u > MAXOUT_12k) 
  {
	u = MAXOUT_12k;
  }
  else if (u < MINOUT_12k) 
  {
	u =  MINOUT_12k;
  }
  
  //set outputs 
  PWMSet((unsigned int)u,1);
  return(0);
}

int PIDSetPoint(volatile struct PIDControl *p, int setpoint)
{
  p->setPoint = setpoint;
  return(0);

}
int AutoCal(volatile struct PIDControl *p)
{

  return(0);
}

