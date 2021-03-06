/****************************************************************************************/
// This class controls the counter peripheral on the ATMega.
//
//   Created : 12 April 2011
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef COUNTER_H_
#define COUNTER_H_

#include <avr/io.h>
#include "types.h"

#ifdef ATMega
#define COUNTER_CLK_STOP        0x00  ///< Counter Stopped
#define COUNTER_CLK_DIV1        0x01  ///< Counter clocked at F_CPU
#define COUNTER_CLK_DIV8        0x02  ///< Counter clocked at F_CPU/8
#define COUNTER_CLK_DIV64       0x03  ///< Counter clocked at F_CPU/64
#define COUNTER_CLK_DIV256      0x04  ///< Counter clocked at F_CPU/256
#define COUNTER_CLK_DIV1024     0x05  ///< Counter clocked at F_CPU/1024
#define COUNTER_CLK_T_FALL      0x06  ///< Counter clocked at T falling edge
#define COUNTER_CLK_T_RISE      0x07  ///< Counter clocked at T rising edge
#endif
#ifdef __AVR_ATmega1280__
#define COUNTER_CLK_STOP        0x00  ///< Counter Stopped
#define COUNTER_CLK_DIV1        0x01  ///< Counter clocked at F_CPU
#define COUNTER_CLK_DIV8        0x02  ///< Counter clocked at F_CPU/8
#define COUNTER_CLK_DIV64       0x03  ///< Counter clocked at F_CPU/64
#define COUNTER_CLK_DIV256      0x04  ///< Counter clocked at F_CPU/256
#define COUNTER_CLK_DIV1024     0x05  ///< Counter clocked at F_CPU/1024
#define COUNTER_CLK_T_FALL      0x06  ///< Counter clocked at T falling edge
#define COUNTER_CLK_T_RISE      0x07  ///< Counter clocked at T rising edge
#define COUNTER_PRESCALE_MASK   0x07  ///< Counter Prescaler Bit-Mask
#endif
#ifdef ATMega0
#define COUNTER_CLK_STOP        0x00  ///< Counter Stopped
#define COUNTER_CLK_DIV1        0x01  ///< Counter clocked at F_CPU
#define COUNTER_CLK_DIV8        0x02  ///< Counter clocked at F_CPU/8
#define COUNTER_CLK_DIV32       0x03  ///< Counter clocked at F_CPU/32
#define COUNTER_CLK_DIV64       0x04  ///< Counter clocked at F_CPU/64
#define COUNTER_CLK_DIV128      0x05  ///< Counter clocked at F_CPU/128
#define COUNTER_CLK_DIV256      0x06  ///< Counter clocked at F_CPU/256
#define COUNTER_CLK_DIV1024     0x07  ///< Counter clocked at F_CPU/1024
#endif


// Different prescale values must be used for timer/counter 2
#define COUNTER2_CLK_DIV32      0x03  ///< Counter 2 clocked at F_CPU/32
#define COUNTER2_CLK_DIV64      0x04  ///< Counter 2 clocked at F_CPU/64
#define COUNTER2_CLK_DIV128     0x05  ///< Counter 2 clocked at F_CPU/128
#define COUNTER2_CLK_DIV256     0x06  ///< Counter 2 clocked at F_CPU/256
#define COUNTER2_CLK_DIV1024    0x07  ///< Counter 2 clocked at F_CPU/1024

class CCounter {
protected:
	u08 devNum;
	u08 prescale;
	u08 ocrna;
	u08 ocrnb;
	u08 icrn;
	u08 tccrna;
	u08 tccrnb;
	u08 tcntn;
	u08 timskn;
public:
	CCounter(u08 devNum, u08 prescale);
	void start(void);
	void stop(void);
};

#endif
