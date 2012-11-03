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

#define CS()                      clrBit(PORTB, 2)
#define nCS()                     setBit(PORTB, 2)
#define MISO_WAIT()               LOOP_UNTIL_BIT_IS_LO(PINB,4);
#define START()                   CS(); MISO_WAIT();
#define STOP()                    nCS();

/****************************************************************************************/
Cspi::Cspi(Cpin* _pinSS, Cpin* _pinMOSI, Cpin* _pinMISO, Cpin* _pinSCK, eSpiMode _mode, eSpiSpeed _spiSpeed , bool msbFirst, bool master, bool clk2X){


  pinMISO = _pinMISO;
  pinSS = _pinSS;
  STOP();

  //_pinMOSI->setLO();
	//_pinSCK->setHI();

  // enable SPI Master, MSB, SPI mode 0, FOSC/4
  SPCR = 0;
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
  if (clk2X) {
    setBit(SPSR, SPI2X);
  } else {
    clrBit(SPSR, SPI2X);
  }
  // enable SPI
  setBit(SPCR, SPE);
  this->mode = _mode;
  setmode(mode);

  switch(_spiSpeed){
    case SPI_SPEED_FOSC_2_2X:
      setBit(SPSR, SPI2X);
    case SPI_SPEED_FOSC_4:
      clrBit(SPCR, SPR0);
      clrBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_8_2X:
      setBit(SPSR, SPI2X);
    case SPI_SPEED_FOSC_16:
      setBit(SPCR, SPR0);
      clrBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_32_2X:
      setBit(SPSR, SPI2X);
    case SPI_SPEED_FOSC_64:
      clrBit(SPCR, SPR0);
      setBit(SPCR, SPR1);
      break;
    case SPI_SPEED_FOSC_64_2X:
      setBit(SPSR, SPI2X);
    case SPI_SPEED_FOSC_128:
      setBit(SPCR, SPR0);
      setBit(SPCR, SPR1);
      break;
  }
}

/****************************************************************************************/
void Cspi::setmode(eSpiMode _mode) {
  /*
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
    */
}

/****************************************************************************************/
u08 Cspi::read(u08 adr) {
  u08 ret;
  adr |= _BV(7);  //RD / nWR  TODO Confirm this is generic to all SPI Reads
  START();
  send(adr);
  ret = send(0x00);
  STOP();
  return ret;
}

/****************************************************************************************/
void Cspi::read(u08 adr, u08* dat, u08 len ) {
  adr |= _BV(7);  //RD / nWR  TODO Confirm this is generic to all SPI Reads
  START();
  send(adr);
  for (u08 i = 0; i < len; i++) {
    dat[i] = send(0x00);
  }
  STOP()
}

/****************************************************************************************/
// Write a value "dat" to a register at adr
/****************************************************************************************/
void Cspi::write(u08 adr, u08 dat) {
  START();
  send(adr);
  send(dat);
  STOP();
}
/****************************************************************************************/
// Write an array of values "dat" to a register at adr
/****************************************************************************************/
void Cspi::write(u08 adr, u08* dat, u08 len ) {
  START();
  send(adr);
  for (u08 i = 0; i < len; i++) {
    send(dat[i]);
  }
  STOP();
}
/****************************************************************************************/
u08 Cspi::strobe(u08 value) {
  u08 ret;

  START();
  ret = send(value);
  STOP()
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
