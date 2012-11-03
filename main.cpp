/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
static const u16 adrPortB = 0x23;
static const u08 radioPktLen =  4;
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
Cpin tagpin(adrPortB, 6, ePinIn, false, true);
Cpin pinSS(adrPortB, 0, ePinOut, true);
Cpin pinSCK(adrPortB, 1, ePinOut, true);
Cpin pinMOSI(adrPortB, 2, ePinOut, true);
Cpin pinMISO(adrPortB, 3, ePinIn, true);
Cpin rfCS(adrPortB, 4, ePinOut, true);
Cpin pinGDO0(adrPortB, 6, ePinIn, true);
Cpin pinGDO2(adrPortB, 7, ePinIn, true);
Csl018 tag(&i2c, &tagpin, 0xA0);
Cspi spi(&pinMOSI, &pinSCK, &pinMISO, &pinSS, &rfCS, SPI_MODE_0, SPI_SPEED_FOSC_64);
CC1101 rf(&spi, &pinGDO0, &pinGDO2, radioPktLen);
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
      if (tag.read()) {

      }
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
