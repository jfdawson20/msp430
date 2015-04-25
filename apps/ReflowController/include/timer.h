#ifndef TIMER_H
#define TIMER_H

int TimerA0Init(); 
int TimerA1Init();
int PWMUpdate(int duty, int out);
int PWMSet(unsigned int duty, int out);

#endif
