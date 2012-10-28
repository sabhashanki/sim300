/*
 * CTimer.cpp
 *
 *  Created on: 17 Apr 2009
 *      Author: Wouter
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrlibdefs.h"
#include "CTimer.h"
#include "common.h"

#define CYCLES_PER_US ((F_CPU+500000)/1000000)  // cpu cycles per microsecond

#ifndef TCCR0B
#define TCCR0B TCCR0
#endif
#ifndef TIMSK0
#define TIMSK0 TIMSK
#endif
#ifndef TIMSK1
#define TIMSK1 TIMSK
#endif

static CTimer* timers[4]= {0,0,0,0};

CTimer::CTimer(u08 devNum, u08 prescale) {
  this->devNum = devNum;
  switch(devNum) {
    case 0:
      TCCR0B = ((TCCR0B & ~TIMER_PRESCALE_MASK) | prescale);
      TCNT0 = 0;
      sbi(TIMSK0, TOIE0);
      break;
#ifdef TCCR1B
    case 1:
      TCCR1B = ((TCCR1B & ~TIMER_PRESCALE_MASK) | prescale);
      TCNT1 = 0;
      sbi(TIMSK1, TOIE1);
      break;
#endif
#ifdef TCCR2B
    case 2:
      TCCR2B = ((TCCR2B & ~TIMER_PRESCALE_MASK) | prescale);
      TCNT2 = 0;
      sbi(TIMSK2, TOIE2);
      break;
#endif
#ifdef TCCR3B
    case 3:
      TCCR3B = ((TCCR3B & ~TIMER_PRESCALE_MASK) | prescale);
      TCNT3 = 0;
      sbi(TIMSK3, TOIE3);
      break;
#endif
  }
}

void CTimer::attach(voidFuncPtr fp)
{
  timers[devNum] = this;
  function = fp;
}

void CTimer::detach(voidFuncPtr fp)
{
  timers[devNum] = 0;
  function = 0;
}

SIGNAL(SIG_OVERFLOW0)
{
  if (timers[0] != 0){
    if (timers[0]->function != 0){
      timers[0]->function();
    }
  }
}
#ifdef TCCR1B
SIGNAL(SIG_OVERFLOW1)
{
  if (timers[1] != 0){
    if (timers[1]->function != 0){
      timers[1]->function();
    }
  }
}
#endif
#ifdef TCCR2B
SIGNAL(SIG_OVERFLOW2)
{
  if (timers[2] != 0){
    if (timers[2]->function != 0){
      timers[2]->function();
    }
  }
}
#endif
#ifdef TCCR3B
SIGNAL(SIG_OVERFLOW3)
{
  if (timers[3] != 0){
    if (timers[3]->function != 0){
      timers[3]->function();
    }
  }
}
#endif



