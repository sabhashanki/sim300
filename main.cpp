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
c08 msg[16];
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
Ci2c i2c;
Cpin tagpin(0x23, 6, ePinIn, false, true);
Csl018 tag(&i2c, &tagpin, 0xA0);
/****************************************************************************************/
void hello() {
  debugUart.sendStr_P(PSTR("\n\r  ===== Manhole Lock System ====="));
  display.writeClear();
  display.writeStringP(PSTR("Initializing..."));
}
/****************************************************************************************/
bool scanRFID() {

}

/****************************************************************************************/
int main(void) {
  init();
  modem.ServerSetIP((c08*) "41.181.16.116", (c08*) "61000", false);
  scheduler.start();
  sei();
  hello();
  display.writeString(PSTR("Init Modem..."));
//  if (!modem.initModem())
//    goto error;
  display.writeString(PSTR("Init Server..."));
//  if (!modem.initIP(false))
//    goto error;
  display.writeStringP(PSTR("Ready: Attach"));
  display.writeStringP(PSTR("to lid..."), 0, 1, false);

  while (1) {
    if (tag.present()) {
      tag.read();
    }
    //modem.service();
    //socket.service();
    //keypad.service();
  }

  error: while (1) {
    display.writeString("ERROR");
  }
  return 0;
}

/****************************************************************************************
 C++ work around
 ****************************************************************************************/
extern "C" void __cxa_pure_virtual(void) {
  // call to a pure virtual function happened ... wow, should never happen ... stop
  while (1)
    ;
}
