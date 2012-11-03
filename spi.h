/****************************************************************************************/
//   SPI driver non-class based
//     Author: PP
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef SPI_H
#define SPI_H
/****************************************************************************************/
#include <stdio.h>
#include <avr/io.h>
/****************************************************************************************/
#include "types.h"
#include "pin.h"
///****************************************************************************************/
typedef enum {
	SPI_MODE_0 = 0,
	SPI_MODE_1 = 1,
	SPI_MODE_2 = 2,
	SPI_MODE_3 = 3
} eSpiMode;
///****************************************************************************************/
typedef enum {
	SPI_SPEED_FOSC_4 = 0,
	SPI_SPEED_FOSC_16 = 1,
	SPI_SPEED_FOSC_64 = 2,
	SPI_SPEED_FOSC_128 = 3,
	SPI_SPEED_FOSC_2_2X = 4,
	SPI_SPEED_FOSC_8_2X = 5,
	SPI_SPEED_FOSC_32_2X = 6,
	SPI_SPEED_FOSC_64_2X = 7
} eSpiSpeed;

/****************************************************************************************/
class Cspi {
  private:
	eSpiMode mode;
	void setmode(eSpiMode _mode);
public:
	Cspi(Cpin* _pinSS, Cpin* _pinMOSI, Cpin* _pinMISO, Cpin* _pinSCK,
        eSpiMode _mode = SPI_MODE_0,
        eSpiSpeed _spiSpeed = SPI_SPEED_FOSC_4,
        bool msbFirst = true,
        bool master = true,
        bool clk2X = false
			);
	Cpin* pinSS;
	Cpin* pinMISO;
	u08 send(u08 value);
	u08 read(u08 adr);
	void read(u08 adr, u08* dat,u08 len );
	void write(u08 adr, u08* dat ,u08 len);
	void write(u08 adr, u08 dat);
	u08 strobe(u08 value);
};
/****************************************************************************************/
#endif
