/****************************************************************************************/
//   SPI driver non-class based
//     Author: PP
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#include "spi.h"
#include "common.h"

/****************************************************************************************/
Cspi::Cspi(Cpin* _pinMOSI, Cpin* _pinSCK, Cpin* _pinMISO, Cpin* _pinSS, Cpin* _pinCS, eSpiMode _mode, eSpiSpeed _spiSpeed , bool msbFirst, bool master){
  pinMISO = _pinMISO;
  // If a different Pin than SPI_SS is used as a chip select
  // Make sure SPI_SS is an output before setting SPI as a master
  if(_pinCS){
    _pinSS->setDir(ePinOut);
    _pinSS->setHigh();
    pinCS = _pinCS;
  }else{
    pinCS = _pinSS;
  }
  pinCS->setDir(ePinOut);
  pinCS->setHigh();
  _pinMOSI->setLow();
  _pinSCK->setHigh();
  stop();

  // Data order MSB first
  if (msbFirst) {
    clrBit(SPCR, DORD);
  } else {
    setBit(SPCR, DORD);
  }
  if (master) {
    setBit(SPCR, MSTR);
  } else {
    clrBit(SPCR, MSTR);
  }
  this->mode = _mode;
  setMode(mode);
  setSpeed(_spiSpeed);
  setBit(SPCR, SPE); // Not really Object orientated - should have a base address here
}

void Cspi::setSpeed(eSpiSpeed _spiSpeed){
  switch(_spiSpeed){
    if(_spiSpeed > SPI_SPEED_FOSC_128){
      setBit(SPSR, SPI2X);
    }
    case SPI_SPEED_FOSC_2_2X:
    case SPI_SPEED_FOSC_4:
      clrBit(SPCR, SPR0);
      clrBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_8_2X:
    case SPI_SPEED_FOSC_16:
      setBit(SPCR, SPR0);
      clrBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_32_2X:
    case SPI_SPEED_FOSC_64:
      clrBit(SPCR, SPR0);
      setBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_64_2X:
    case SPI_SPEED_FOSC_128:
      setBit(SPCR, SPR0);
      setBit(SPCR, SPR1);
      break;
  }

}
/****************************************************************************************/
void Cspi::setMode(eSpiMode _mode) {
  switch(_mode){
      case SPI_MODE_0:
        clrBit(SPCR, CPOL);
        clrBit(SPCR, CPHA);
        break;
      case SPI_MODE_1:
        clrBit(SPCR, CPOL);
        setBit(SPCR, CPHA);
        break;
      case SPI_MODE_2:
        setBit(SPCR, CPOL);
        clrBit(SPCR, CPHA);
        break;
      case SPI_MODE_3:
        setBit(SPCR, CPOL);
        setBit(SPCR, CPHA);
        break;
    }
}

/****************************************************************************************/
u08 Cspi::read(u08 adr) {
  u08 ret;
  adr |= _BV(7);  //RD / nWR  TODO Confirm this is generic to all SPI Reads
  start();
  send(adr);
  ret = send(0x00);
  stop();
  return ret;
}

/****************************************************************************************/
void Cspi::read(u08 adr, u08* dat, u08 len ) {
  adr |= _BV(7);  //RD / nWR  TODO Confirm this is generic to all SPI Reads
  start();
  send(adr);
  for (u08 i = 0; i < len; i++) {
    dat[i] = send(0x00);
  }
  stop();
}

/****************************************************************************************/
// Write a value "dat" to a register at adr
/****************************************************************************************/
void Cspi::write(u08 adr, u08 dat) {
  start();
  send(adr);
  send(dat);
  stop();
}
/****************************************************************************************/
// Write an array of values "dat" to a register at adr
/****************************************************************************************/
void Cspi::write(u08 adr, u08* dat, u08 len ) {
  start();
  send(adr);
  for (u08 i = 0; i < len; i++) {
    send(dat[i]);
  }
  stop();
}
/****************************************************************************************/
u08 Cspi::strobe(u08 value) {
  u08 ret;

  start();
  ret = send(value);
  stop();
  return ret;
}
/****************************************************************************************/
u08 Cspi::send(u08 value) {
  SPDR = value;
  //When a serial transfer is complete, the SPIF Flag is set wait for this to happen
  while(!(SPSR & _BV(SPIF)));
  // SPIF bit is cleared when SPDR is read
  return SPDR;
}
