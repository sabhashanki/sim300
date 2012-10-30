//****************************************************************************************
// A simple class to setup and control an AVR port pin.
//
//   Created : 08 Apr 2011
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
//****************************************************************************************
#ifndef PIN_H_
#define PIN_H_
//****************************************************************************************
#include "types.h"
//****************************************************************************************
#define PIN_ADR     (this->pinAdr)
#define DDR_ADR     (this->pinAdr+1)
#define PORT_ADR    (this->pinAdr+2)
//****************************************************************************************
typedef enum {
  ePinLow = 0, ePinHigh = 1
} ePinState;
//****************************************************************************************
typedef enum {
  ePinIn = 0, ePinOut = 1
} ePinDir;
//****************************************************************************************
class Cpin {
    bool activeLow;
    u08 pin;
    u16 pinAdr;
  public:
    Cpin(u16 portBaseAdr, u08 pinNumber, ePinDir dir, bool pullup = false, bool activeLow = false);
    bool isEnabled(void);
    bool isDisabled(void);
    void setEnable(void);
    void setDisable(void);
    void toggle(void);
};
//****************************************************************************************
#endif
