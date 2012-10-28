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
#include "main.h"
#include "CModem.h"
#include "common.h"
#include "iopins.h"
#include "CTimer.h"
#include "CUART.h"
#include "CRfid.h"
#include "CI2C.h"
#include "Crtc.h"
#include "CServer.h"
#include "CController.h"
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
CUART ModemUart(2, 115200, 1024);
//CModem Modem(&ModemUart,"cloete.vm.bytemark.co.uk","55555");
CModem Modem(&ModemUart);
//CModem Modem(&ModemUart,"xdm.dyndns.org","55555",true);
//CModem Modem(&ModemUart,"www.google.com","80");
//CModem Modem(&ModemUart,"41.125.130.119","80",false);
CI2C i2c;
Crtc rtc(&i2c, 0xD0);
CServer Server(&Modem);
//CRFID Rfid(&RfidUart, 16, &Server);
//CController Controller(&Rfid, &Modem, &Server);
sTimeDate rtc_time;
/****************************************************************************************/
int main(void) {
  u08 dat[128];
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

  while (1) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      time = isr_time;
    }
    //Rfid.Service();
    //Controller.Service();
    Modem.Init();
    Modem.connect(false);

    if (time > 200000) {
      time = 0;

      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        isr_time = 0;
      }
      LED_3_TOGGLE();
      if (Modem.simcard_ok) {
        LED_0_TOGGLE();
      } else {
        LED_0_OFF();
      }
      if (Modem.signal_ok) {
        LED_1_TOGGLE();
      } else {
        LED_1_OFF();
      }
      if (Modem.connect_ok) {
        LED_2_TOGGLE();
      } else {
        LED_2_OFF();
      }
    }
  }
  return 0;
}

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
