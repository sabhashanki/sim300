/****************************************************************************************/
// This class controls the timer peripheral on the ATMega.
//
//   Created : 12 April 2011
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef CTIMER_H_
#define CTIMER_H_

#include <avr/io.h>
#include "types.h"
#include "counter.h"

typedef enum {
  SINGLE,       //One-shot timer
    CONTINUOUS  //Timer resets itself automatically
} eTimerMode;

typedef enum {
  OVERFLOW,   //Overflow
    COMPAREA, //Output Compare A
  COMPAREB    //Output Compare B
} eTimerIntr;

class Ctimer: public CCounter {
    u32 time;
    u08 ocr;
    eTimerMode mode;
    void (*handler)(void);
  public:
    u32 count;
    Ctimer(u08 devNum, u08 prescale, eTimerMode mode);
    void setModeCTC(u08 count);
    void disableInterrupt(eTimerIntr intr);
    void enableInterrupt(eTimerIntr intr);
    void attach(void (*userHandler)(void));
    void start(u32 time);
    void start(u32 time, void (*userHandler)(void));
    void service(void);
    u08 getTime(void) {
      return _MMIO_BYTE(this->tcntn);
    }
};

#endif
