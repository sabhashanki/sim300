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
Cpin::Cpin(u16 _portBaseAdr, u08 _pinNumber, ePinDir _dir, bool _pullup, bool _activeLow) {
  this->pinAdr = _portBaseAdr;
  this->pin = _pinNumber;
  this->isActiveLow = _activeLow;
  if (_dir == ePinIn) {
    // Set the pin as an input by clearing the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) &= ~(1 << _pinNumber);
  } else {
    // Set the pin as an output by setting the bit in the direction register.
    (*(volatile u08*) (DDR_ADR)) |= (1 << _pinNumber);
  }
  // If the pullup needs to be on configure it.
  if (_pullup) {
    (*(volatile u08*) (PORT_ADR)) |= (1 << _pinNumber);
  } else {
    (*(volatile u08*) (PORT_ADR)) &= ~(1 << _pinNumber);
  }
}
//****************************************************************************************
void Cpin::setDir(ePinDir _dir) {
  if (_dir == ePinIn) {
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
bool Cpin::isHigh(void) {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x1);
}
//****************************************************************************************
bool Cpin::isLow(void) {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x0);
}
//****************************************************************************************
void Cpin::setHigh(void) {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
}
//****************************************************************************************
void Cpin::setLow(void) {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
}
//****************************************************************************************
void Cpin::setPolarity(bool _isActiveLow) {
  isActiveLow = _isActiveLow;
}
//****************************************************************************************
bool Cpin::isEnabled(void) {
  if (isActiveLow) {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x0);
  } else {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x1);
  }
}
//****************************************************************************************
bool Cpin::isDisabled(void) {
  if (isActiveLow) {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x1);
  } else {
    return ((((*(volatile u08*) (PIN_ADR)) >> pin) & 0x1) == 0x0);
  }
}
//****************************************************************************************
void Cpin::enable(void) {
  if (!isActiveLow) {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::disable(void) {
  if (!isActiveLow) {
    *(volatile u08*) (PORT_ADR) &= ~(0x1 << pin);
  } else {
    *(volatile u08*) (PORT_ADR) |= (0x1 << pin);
  }
}
//****************************************************************************************
void Cpin::toggle(void) {
  if (isDisabled()) {
    enable();
  } else {
    disable();
  }
}
//****************************************************************************************
