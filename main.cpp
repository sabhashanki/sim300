/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
static const u16 adrPortB = 0x23;
static const u08 radioPktLen = 8;
static const u08 open = 0xCC;
static const u08 close = 0xDD;
static const u08 moveTime = 5;
/****************************************************************************************/
c08 msg[16];
Cuart debugUart(0, 115200);
Cuart modemUart(2, 115200);
Cmodem modem(&modemUart);
Csocket socket(&modem);
Cserver server(&socket);
Cuart keypadUart(3, 9600, 0x60, true);
Cnetwork network(&keypadUart, 0xFA, 0x60);
Ctransport transport(&network);
Cdisplay display(&transport);
Ckeypad keypad(&transport);
Ci2c i2c;
Cinput tagpin(adrPortB, 6, ePinActiveLow, ePinPullup);
Csl018 tag(&i2c, &tagpin, 0xA0);

Coutput pinSS(adrPortB, 0, ePinActiveHigh, ePinHigh);
Coutput pinSCK(adrPortB, 1, ePinActiveHigh, ePinLow);
Coutput pinMOSI(adrPortB, 2, ePinActiveHigh, ePinLow);
Cinput pinMISO(adrPortB, 3, ePinActiveHigh, ePinPullup);
Coutput rfCS(adrPortB, 4, ePinActiveLow, ePinHigh);
Cspi spi(&pinMOSI, &pinSCK, &pinMISO, &pinSS, &rfCS, SPI_MODE_0, SPI_SPEED_FOSC_64);

Cinput pinGDO0(adrPortB, 6, ePinActiveLow, ePinPullup);
Cinput pinGDO2(adrPortB, 7, ePinActiveLow, ePinPullup);
CC1101 rf(&spi, &pinGDO0, &pinGDO2, radioPktLen);
u08 radio[32];
/****************************************************************************************/
void testRf() {
  u08 radio[32];
  u08 cmd;

  if (debugUart.rxFifo.read(&cmd, 1) == 1) {
    if (cmd == 'o') {
      debugUart.sendStr_P(PSTR("\n\rOPEN THE LOCK"));
      radio[0] = 0xCC;
      radio[1] = 5;
      rf.transmit(radio, 2);
      _delay_ms(500);
    }
    if (cmd == 'c') {
      debugUart.sendStr_P(PSTR("\n\rCLOSE THE LOCK"));
      radio[0] = 0xDD;
      radio[1] = 5;
      rf.transmit(radio, 2);
      _delay_ms(500);
    }
  }
}
/****************************************************************************************/
int main(void) {
  init();
  modem.ServerSetIP((c08*) "41.181.16.116", (c08*) "61000", false);
  scheduler.start();
  sei();
  debugUart.sendStr_P(PSTR("\n\r  ===== Manhole Lock System ====="));
  display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Initializing..."), 0, 1, false);
  display.writeStringP(PSTR("Busy with GSM Modem"), 0, 2, false);
  _delay_ms(500);
//  if (!modem.initModem())
//    goto error;
  display.writeStringP(PSTR("Done with GSM Modem"), 0, 2, false);
  display.writeStringP(PSTR("Contacting server"), 0, 3, false);
  _delay_ms(500);
//  if (!modem.initIP(false))
//    goto error;
  display.writeStringP(PSTR("Connected to Server"), 0, 3, false);

  _delay_ms(500);
  display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Ready"), 0, 1, false);
  display.writeStringP(PSTR("Place unit on lid"), 0, 2, false);
  display.writeStringP(PSTR("..."), 0, 3, false);

  tag.reset();
  while (!tag.present()) {
    _delay_ms(10);
  }
  while (!tag.read()) {
    _delay_ms(10);
  }
  display.writeStringP(PSTR("Lid detected !!!"), 0, 3, false);
  _delay_ms(500);

  display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Ready"), 0, 1, false);
  display.writeStringP(PSTR("Reading ID"), 0, 2, false);
  display.writeStringP(PSTR("..."), 0, 3, false);
  rf.rxFifo.clear();
  rf.packetAvailable = false;
  rf.setRxMode();
  _delay_ms(200);
  while (!rf.packetAvailable) {
    rf.rxISR();
    _delay_ms(200);
  }
  rf.rxFifo.read(radio, radioPktLen);
  display.writeStringP(PSTR("ID read OK !!!  "), 0, 3, false);

  display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Ready"), 0, 1, false);
  display.writeStringP(PSTR("PRESS OPEN"), 0, 2, false);





  radio[1] = moveTime;
  while (1) {
    radio[0] = open;
    rf.transmit(radio, radioPktLen);
    radio[0] = close;
    rf.transmit(radio, radioPktLen);
  }

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
