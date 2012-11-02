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
#include "pin.h"
/****************************************************************************************/
#define PIN_ADR     (this->pinAdr)
#define DDR_ADR     (this->pinAdr+1)
#define PORT_ADR    (this->pinAdr+2)
/****************************************************************************************/
Cpin::Cpin(u16 portBaseAdr, u08 pinNumber, ePinDir dir, bool _pullup, bool _activeLow) {
  this->pinAdr = portBaseAdr;
  this->pin = pinNumber;
  this->activeLow = _activeLow;
  if (dir == ePinIn) {
    // Set the pin as an input by clearing the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) &= ~(1 << pinNumber);
  } else {
    // Set the pin as an output by setting the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) |= (1 << pinNumber);
  }
  // If the pullup needs to be on configure it.
  if (_pullup) {
    (*(volatile u08*) (PORT_ADR)) |= (1 << pinNumber);
  } else {
    (*(volatile u08*) (PORT_ADR)) &= ~(1 << pinNumber);
  }
}
//****************************************************************************************
void Cpin::setDir(ePinDir dir) {
  if (dir == ePinIn) {
    // Set the pin as an input by clearing the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) &= ~(1 << pin);
  } else {
    // Set the pin as an output by setting the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) |= (1 << pin);
  }
}
//****************************************************************************************
void  Cpin::pullup(bool _pullup) {
  // If the pullup needs to be on configure it.
  if (_pullup) {
    (*(volatile u08*) (PORT_ADR)) |= (1 << pin);
  } else {
    (*(volatile u08*) (PORT_ADR)) &= ~(1 << pin);
  }
}
//****************************************************************************************
void Cpin::setlogic(bool _activeLow) {
  activeLow = _activeLow;
}
//****************************************************************************************
bool Cpin::isSet(void) {
  if (activeLow) {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x0);
  } else {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x1);
  }
}
//****************************************************************************************
bool Cpin::isClr(void) {
  if (activeLow) {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x1);
  } else {
    return ((((*(volatile u08*) (this->pinAdr)) >> pin) & 0x1) == 0x0);
  }
}
//****************************************************************************************
void Cpin::set(void) {
  if (!activeLow) {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::clr(void) {
  if (!activeLow) {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::toggle(void) {
  if (isClr()) {
    set();
  } else {
    clr();
  }
}
//****************************************************************************************
