/****************************************************************************************/
// The Csocket class maintains a socket connection to a server.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef CSOCKET_H
#define CSOCKET_H
/****************************************************************************************/
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/****************************************************************************************/
#include "common.h"
#include "fifo.h"
#include "pin.h"
#include "serial.h"
#include "scheduler.h"
#include "modem.h"
/****************************************************************************************/
namespace CSOCKET {
  const u32 LIFETIME = (5); // 5 Second timeout
}
/****************************************************************************************/
class Csocket: public Cserial {
    u08 socketNr;
    u16 port;
    bool print;
    u32 idleTime;
    Csignal signal;
    u08* buf;
    bool autoclose;
    Cmodem* modem;
  public:
    Csocket(Cmodem* modem, u16 bufSize = 128, bool _autoclose = true);
    void service(void);
    u16 write(u08* buffer, u16 nBytes);
    u16 read(u08* buffer, u16 nBytes);
    bool start(void);
    void enableAutoclose();
    void disableAutoclose() {
      autoclose = false;
    }
};
/****************************************************************************************/
#endif

