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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
/****************************************************************************************/
#include "types.h"
#include "Csocket.h"
#include "iopins.h"
#include "common.h"
/****************************************************************************************/
using namespace CSOCKET;
/****************************************************************************************/
//u08 debugClose = false;
//u08 debugSend = true;
/****************************************************************************************/
Csocket::Csocket(Cmodem* _modem, u16 _bufSize, bool _autoclose) {
  modem = _modem;
  this->socketNr = socketNr;
  this->port = port;
  healthy = true;
  setBufSize(_bufSize);
  rxOverflowCnt = 0;
  txBusy = false;
  print = true;
  //dst = dstIP;
  signal.setPeriod(1 / Csignal::tickBase);
  scheduler.attach(&signal);
  autoclose = _autoclose;
}
/****************************************************************************************/
void Csocket::service(void) {
  u32 len;
  if (signal.isSet()) {
    switch (modem->ss) {
      case SOCK_ESTABLISHED:
        if (txFIFO.used() > 0) {
          if (modem->send(&txFIFO)) {
            idleTime = 0;
          }
        }
        if ((len = modem->rxFifo.used()) > 0) {
          modem->rxFifo.read(&rxFIFO);
          modem->disconnect();
          idleTime = 0;
        }
        if (autoclose) {
          if (idleTime > LIFETIME) {
            modem->disconnect();
          }
        }
        if (signal.isSet()) {
          idleTime++;
        }
        break;
      case SOCK_CLOSE_WAIT:
        modem->disconnect();
        break;
      case SOCK_CLOSED: // CLOSED
        if (!txFIFO.empty()) {
          idleTime = 0;
          modem->connect();
        }
        break;
      case SOCK_INIT: // The SOCKET opened with TCP mode
        //connectSocket(socketNr, (u08*) &dst, port);
        break;
      default:
        break;
    }
  }
}
/****************************************************************************************/
u16 Csocket::send(u08* buffer, u16 nBytes) {
  u16 res;
  if (!nBytes || !buffer) {
    return 0;
  }
  res = txFIFO.write(buffer, nBytes);
  return res;
}
/****************************************************************************************/
bool Csocket::start(void) {
  healthy = false;
  return true;
}
/****************************************************************************************/
void Csocket::enableAutoclose() {
  idleTime = 0;
  autoclose = true;
}
/****************************************************************************************/
