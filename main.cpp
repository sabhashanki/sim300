/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
static volatile u32 isr_time = 0;
static volatile u32 time = 0;
bool testSend = true;
/****************************************************************************************/
#define TICKER_PERIOD  139;
/****************************************************************************************/
void TimeTicker(void);
/****************************************************************************************/
//CTimer Timer(0, TIMER_CLK_DIV8);
Cuart debugUart(0, 115200);
Cuart modemUart(2, 115200);
Cmodem modem(&modemUart);
Csocket socket(&modem);
Cserver server(&socket);
Cuart keypadUart(1);
Cnetwork network(&keypadUart, 1);
Ctransport transport(&network);
Cdisplay display(&transport);
Ckeypad keypad(&transport);
//Ci2c i2c;
//Crtc rtc(&i2c, 0xD0);
//CServer Server(&Modem);
//CRFID Rfid(&RfidUart, 16, &Server);
//CController Controller(&Rfid, &Modem, &Server);
//sTimeDate rtc_time;
/****************************************************************************************/
int main(void) {
  //=== Enable the External RAM
  //BIT_SET_HI(XMCRA, SRE);
  InitIOPins();
  WDTCSR = 0x00; // Disable Watchdog for now
  //Timer.attach(TimeTicker);
  scheduler.start();
  sei();
  debugUart.sendStr_P(PSTR("\x1B[2J")); //Clear Screen
  debugUart.sendStr_P(PSTR("\x1B[0;0H")); //Position Cursor
  debugUart.sendStr_P(PSTR("\n\r  ===== Manhole Lock System ===== \n\r"));
//  DbgUart.sendStr(rtc.getTimestamp());
  //}
  //Controller.Setup();
//  modem.ServerSetIP((c08*) "41.181.16.116", (c08*) "61000", false);
//  if (!modem.initModem())
//    goto error;
//  if (!modem.initIP(false))
//    goto error;

  while (1) {
    modem.service();
    socket.service();
    keypad.service();
  }

  error: while (1) {
    testSend = !testSend;
  }
  return 0;
}

/****************************************************************************************/

void TimeTicker(void) {
  //Rfid.timer += TICKER_PERIOD;
  isr_time += TICKER_PERIOD
  ;
  modem.isr_timer += TICKER_PERIOD
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
