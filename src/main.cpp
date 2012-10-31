/****************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
/****************************************************************************************/
#include "network.h"
#include "main.h"
#include "CModem.h"
#include "common.h"
#include "iopins.h"
#include "CTimer.h"
#include "CUART.h"
#include "CRfid.h"
#include "i2c.h"
//#include "Crtc.h"
#include "CServer.h"
#include "CController.h"
#include "Csocket.h"
/****************************************************************************************/
static volatile u32 isr_time = 0;
static volatile u32 time = 0;
/****************************************************************************************/
#define TICKER_PERIOD  139;
using namespace CCONTROLLER;
/****************************************************************************************/
void TimeTicker(void);
/****************************************************************************************/
//CTimer Timer(0, TIMER_CLK_DIV8);
CUART DbgUart(0, 115200, 255);
CUART ModemUart(2, 115200, 255);
CModem Modem(&ModemUart);
Csocket socket(&Modem);
CNetwork server(&socket, 255);
//Ci2c i2c;
//Crtc rtc(&i2c, 0xD0);
//CServer Server(&Modem);
//CRFID Rfid(&RfidUart, 16, &Server);
//CController Controller(&Rfid, &Modem, &Server);
//sTimeDate rtc_time;
/****************************************************************************************/
int main(void) {
  u08 dat[128];
  u32 testCnt = 0;
   //=== Enable the External RAM
  //BIT_SET_HI(XMCRA, SRE);
  InitIOPins();
  WDTCSR = 0x00; // Disable Watchdog for now
  //Timer.attach(TimeTicker);
  sei();
  DbgUart.sendStr_P(PSTR("\x1B[2J")); //Clear Screen
  DbgUart.sendStr_P(PSTR("\x1B[0;0H")); //Position Cursor
  //while(1){
  DbgUart.sendStr_P(PSTR("\n\r  ===== CULLINAN RFID MANAGER V1.5 DEMO ===== \n\r"));
//  DbgUart.sendStr(rtc.getTimestamp());
  //}
  //Controller.Setup();
  Modem.ServerSetIP((c08*) "41.181.16.116", (c08*) "61000", false);
  if (!Modem.initModem())
    goto error;
  if (!Modem.initIP(false))
    goto error;

  while (1) {
    Modem.service();
    socket.service();
    testCnt++;
    if (testCnt > 4000000) {
      socket.send(dat, 30);
      testCnt = 0;
      server.sendTestPkt();
    }
  }
  error: while (1) {
  }
  return 0;
}

/****************************************************************************************/

void TimeTicker(void) {
  //Rfid.timer += TICKER_PERIOD;
  isr_time += TICKER_PERIOD
  ;
  Modem.isr_timer += TICKER_PERIOD
  ;
  //Controller.timer += TICKER_PERIOD;
}
/****************************************************************************************
 C++ work around
 ****************************************************************************************/
extern "C" void __cxa_pure_virtual(void) {
  // call to a pure virtual function happened ... wow, should never happen ... stop
  while (1)
    ;
}
