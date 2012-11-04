/****************************************************************************************/
#include "main.h"
/****************************************************************************************/
static const u16 adrPortB = 0x23;
static const u08 radioPktLen = 8;
static const u08 open = 0xCC;
static const u08 close = 0xDD;
static const u08 moveTime = 40;
/****************************************************************************************/
c08 msg[16];
u16 openTime;
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
u08 key;
Csignal timeout;
eCoverStatus status = COVER_CLOSED;
/****************************************************************************************/
int main(void) {
  init();
  scheduler.attach(&timeout);
  modem.ServerSetIP((c08*) "41.181.16.116", (c08*) "61000", false);
  scheduler.start();
  sei();

  retry: display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Ready"), 0, 1, false);
  display.writeStringP(PSTR("PRESS OPEN"), 0, 2, false);

  debugUart.sendStr_P(PSTR("\n\r  ===== Manhole Lock System ====="));
  display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
  display.writeStringP(PSTR("Initializing..."), 0, 1, false);
  display.writeStringP(PSTR("GSM Modem"), 0, 2, false);
  display.writeStringP(PSTR("Busy..."), 0, 3, false);
  _delay_ms(500);
  if (!modem.initModem())
    goto retry;
  display.writeStringP(PSTR("Done    "), 0, 3, false);
  _delay_ms(500);
  display.writeStringP(PSTR("Remote Server"), 0, 2, false);
  display.writeStringP(PSTR("Busy..."), 0, 3, false);
  if (!modem.initIP(false))
    goto retry;
  if(!modem.connect())
    goto retry;
  display.writeStringP(PSTR("Done    "), 0, 3, false);

//  status = COVER_OPEN;
//  while (1) {
//    server.sendCoverStatus(status);
//    if (status == COVER_OPEN)
//      status = COVER_CLOSED;
//    else
//      status = COVER_OPEN;
//    _delay_ms(20);
//  }
  while (1) {
    _delay_ms(500);
    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
    display.writeStringP(PSTR("Ready"), 0, 1, false);
    display.writeStringP(PSTR("Place unit on cover"), 0, 2, false);
    display.writeStringP(PSTR("Waiting..."), 0, 3, false);

    tag.reset();
    while (!tag.present()) {
      _delay_ms(10);
    }
    while (!tag.read()) {
      _delay_ms(10);
    }
    display.writeStringP(PSTR("Done       "), 0, 3, false);
    _delay_ms(500);

    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
    display.writeStringP(PSTR("Ready"), 0, 1, false);
    display.writeStringP(PSTR("Reading ID"), 0, 2, false);
    display.writeStringP(PSTR("Busy..."), 0, 3, false);
    rf.rxFifo.clear();
    rf.packetAvailable = false;
    rf.setRxMode();
    _delay_ms(200);
    while (!rf.packetAvailable) {
      rf.rxISR();
      _delay_ms(200);
    }
    rf.rxFifo.read(radio, radioPktLen);
    display.writeStringP(PSTR("Done  "), 0, 3, false);
    _delay_ms(1000);

    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
    display.writeStringP(PSTR("Ready"), 0, 1, false);
    display.writeStringP(PSTR("PRESS 1 TO OPEN"), 0, 2, false);
    display.writeStringP(PSTR("PRESS 2 TO CLOSE"), 0, 3, false);
    key = 0;
    while (!keypad.readKey(&key)) {

    }
//    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
//    display.writeStringP(PSTR("Ready"), 0, 1, false);
//    display.writeStringP(PSTR("Getting Permission"), 0, 2, false);
//    display.writeStringP(PSTR("from server ..."), 0, 3, false);
//    _delay_ms(3000);

    if (key == 1) {
      display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
      display.writeStringP(PSTR("Ready"), 0, 1, false);
      display.writeStringP(PSTR("Opening lock"), 0, 2, false);
      display.writeStringP(PSTR("..."), 0, 3, false);

      radio[1] = moveTime;
      radio[0] = open;
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      rf.setRxMode();
      _delay_ms(6000);
      rf.rxFifo.clear();
      rf.packetAvailable = false;
      timeout.start(moveTime);
      while (!rf.packetAvailable) {
        rf.rxISR();
        _delay_ms(200);
      }
      status = COVER_OPEN;
    } else if (key == 2) {
      display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
      display.writeStringP(PSTR("Ready"), 0, 1, false);
      display.writeStringP(PSTR("Closing lock"), 0, 2, false);
      display.writeStringP(PSTR("..."), 0, 3, false);

      radio[1] = moveTime;
      radio[0] = close;
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      _delay_ms(300);
      rf.transmit(radio, radioPktLen);
      rf.setRxMode();
      _delay_ms(6000);
      rf.rxFifo.clear();
      rf.packetAvailable = false;
      timeout.start(moveTime);
      while (!rf.packetAvailable) {
        rf.rxISR();
        _delay_ms(200);
      }
      status = COVER_CLOSED;
    }

    // Log to server
    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
    display.writeStringP(PSTR("Ready"), 0, 1, false);
    display.writeStringP(PSTR("Talking to Server"), 0, 2, false);
    display.writeStringP(PSTR("Busy..."), 0, 3, false);

    server.sendCoverStatus(status);
    display.writeStringP(PSTR("Done"), 0, 3, false);
    _delay_ms(1000);

    // Log to server
    if (key == 1) {
      display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
      display.writeStringP(PSTR("Ready"), 0, 1, false);
      display.writeStringP(PSTR("Lock is Open"), 0, 2, false);
      display.writeStringP(PSTR("Remove the cover"), 0, 3, false);
    } else if (key == 2) {
      display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
      display.writeStringP(PSTR("Ready"), 0, 1, false);
      display.writeStringP(PSTR("Lock is closed"), 0, 2, false);
      display.writeStringP(PSTR("Remove the unit"), 0, 3, false);
    }

    tag.reset();
    while (tag.present()) {
      _delay_ms(10);
    }
  }
  error: while (1) {
    display.writeStringP(PSTR("Manhole Lock System"), 0, 0);
    display.writeStringP(PSTR("Error"), 0, 1, false);
    display.writeStringP(PSTR("Turn power off"), 0, 2, false);
    display.writeStringP(PSTR("and on again !"), 0, 3, false);
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
