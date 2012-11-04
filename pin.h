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
typedef struct {
    ePinState _state;
} sOutput;
//****************************************************************************************
typedef struct {
    bool pullup;
} sInput;
//****************************************************************************************
typedef ePinState tInput;
typedef bool tOutput;
//****************************************************************************************
class Cpin {
    bool isActiveLow;
    u08 pin;
    u16 pinAdr;
  public:
    Cpin(u16 _portBaseAdr, u08 _pinNumber, ePinDir _dir, bool _pullup, bool _activeLow);
    Cpin(u16 portBaseAdr, u08 _pinNumber, tInput _state, bool _isActiveLow = false);
    Cpin(u16 portBaseAdr, u08 _pinNumber, tOutput _pullup = false, bool _isActiveLow = false);
        bool isEnabled(void);
    bool isDisabled(void);
    void enable(void);
    void disable(void);
    bool isHigh(void);
    bool isLow(void);
    void setHigh(void);
    void setLow(void);
    void setPolarity(bool _isActiveLow);
    void toggle(void);
    void setDir(ePinDir _dir);
    void pullup(bool _pullup);
};
//****************************************************************************************
#endif
