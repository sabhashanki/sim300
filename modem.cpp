#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "common.h"
#include "types.h"
#include "uart.h"
#include "timer.h"
#include "modem.h"
#include "iopins.h"
#include "main.h"

#define DYNDNS_IP   "204.13.248.74"

const char AT_CALL_READY[] = "Call Ready";
const char AT_OK[] = "\r\nOK\r\n";
const char AT_IP[] = ".";
const char SHUT_OK[] = "SHUT OK";
const char AT_RDY[] = ">";
const char AT_NONE[] = "";
const char AT_SEND_OK[] = "SEND OK";
const char AT_CONNECT[] = "CONNECT OK";
const char AT_ERROR[] = "ERROR";
const char AT_DATA[] = "\r\n";
const char SIM_RDY[] = "+CPIN: READY";
const char AT_IP_CLOSE[] = "CLOSE OK";

extern Cuart debugUart;

#define DEBUG_PRINT
/*******************************************************************************/
Cmodem::Cmodem(Cuart * _pUart, u08 bufSize) :
    Csignal(period) {
  usedns = false;
  pUart = _pUart;
  strcpy(serverIP, "");
  strcpy(port, "");
  ss = SOCK_CLOSED;
  rxFifo.setBufSize(bufSize);
  scheduler.attach(this);
  scheduler.attach(&timeout);
}
/*******************************************************************************/
bool Cmodem::HandleAtCmd(c08* _cmd, const char* _expRsp) {

  const u08 len = 64;
  c08 rxRsp[len];
  bool done;
  u08 cnt;

  for (cnt = 0; cnt < numRetries; cnt++) {
    done = false;
    memset(rxRsp, 0, len);
    pUart->clear();
#ifdef DEBUG_PRINT
    debugUart.uprintf("\r\nCmd : %s", _cmd);
#endif
    pUart->sendStr(_cmd);
    timeout.start(10);
    while (!timeout.isSet() && !done) {
      _delay_ms(200);
      pUart->rxFifo.read((u08*) rxRsp, len, true);
      if (strchr(rxRsp, '\r'))
        done = true;
    }
#ifdef DEBUG_PRINT
    debugUart.uprintf("\r\nResponse pkt: %s", rxRsp);
#endif
    if (done) {
      if (strstr(rxRsp, _expRsp)) {
        //debugUart.uprintf("\r\nDONE : %s", _cmd);
        _delay_ms(2000);
        return true;
      }
    }
    _delay_ms(1000);
  }
  return false;
}
/*******************************************************************************/
bool Cmodem::checkSignalStrength() {
  const u08 len = 64;
  c08 rxRsp[len];
  c08* pStr;
  c08* str;
  u08 signalStrenght;
  bool done;
  u08 cnt;

  for (cnt = 0; cnt < numRetries; cnt++) {
    done = false;
    memset(rxRsp, 0, len);
    pUart->clear();
    pUart->sendStr_P(PSTR("AT+CSQ\r"));
    timeout.start(1);
    while (!timeout.isSet() && !done) {
      _delay_ms(200);
      pUart->rxFifo.read((u08*) rxRsp, 0, true);
      if (strchr(rxRsp, '\r'))
        done = true;
    }

#ifdef DEBUG_PRINT
    debugUart.uprintf("\r\nSignal strength pkt: %s", rxRsp);
#endif
    if (done) {
      if (strstr_P(rxRsp, PSTR("+CSQ"))) {
        pStr = rxRsp;
        str = strsep(&pStr, ":");
        str = strsep(&pStr, ",");
        signalStrenght = atoi(str);
        if (signalStrenght < 99 && signalStrenght > 5) {
#ifdef DEBUG_PRINT
          debugUart.uprintf("\r\nSignal strength: %i", signalStrenght);
#endif
          return true;
        }
        if (signalStrenght == 99) {
          cnt = 0;
          _delay_ms(1000);
        }
      }
    }
  }
  return false;
}
/*******************************************************************************/
bool Cmodem::initModem(void) {
  u08 rc = 0;

  retry: if (rc > 4) {
    return false;
  }
  debugUart.sendStr_P(PSTR("\r\nSTART: GSM"));
  rc++;
  PowerOff();
  PowerOn();
  CLR_DTR();
  CLR_RTS();
  _delay_ms(200);
  SET_DTR();
  _delay_ms(200);
  SET_RTS();
  while (GET_CTS_STATUS()) {
    _delay_ms(5);
  }
  _delay_ms(5000);
  HandleAtCmd("ATE0\r", "ATE0");
//  debugUart.sendStr_P(PSTR("\r\nRETRY: GSM"));
  if (!HandleAtCmd("AT\r", AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT'"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CIPHEAD=1\r", AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CIPHEAD=1'"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CMGF=1\r", AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CMGF=1'"));
    goto retry;
  }
  if (!checkSignalStrength()) {
    debugUart.sendStr_P(PSTR("\r\nERROR: SIGNAL STRENGTH"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CPIN?\r", "+CPIN: READY")) {
    debugUart.sendStr_P(PSTR("\r\nERROR: CHECK SIM"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CREG=2\r", "OK")) {
    debugUart.sendStr_P(PSTR("\r\nERROR: REGISTRATION"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CREG?\r", "+CREG: 2,1")) {
    debugUart.sendStr_P(PSTR("\r\nERROR: REGISTRATION"));
    goto retry;
  }
  _delay_ms(2000);

  if (!HandleAtCmd("AT+CREG=0\r", "OK")) {
    debugUart.sendStr_P(PSTR("\r\nERROR: REGISTRATION"));
    goto retry;
  }
  debugUart.sendStr_P(PSTR("\r\nDONE: GSM"));
  return true;
}
/*******************************************************************************/
bool Cmodem::initIP(bool useDns) {
  c08 txcmd[bufSize];
  u08 rc = 0;

  retry: if (rc > 4) {
    return false;
  }
  debugUart.sendStr_P(PSTR("\r\nSTART: TCP"));
  rc++;
  if (!HandleAtCmd("AT+CGDCONT=1,\"IP\",\"internet\"\r", AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CGDCONT=1'"));
    goto retry;
  }
  if (!HandleAtCmd("AT+CDNSORIP=0\r", AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CDNSORIP'"));
    goto retry;
  }
  strcpy_P(txcmd, PSTR("AT+CSTT=\"internet\",\"\",\"\"\r"));
  if (!HandleAtCmd(txcmd, AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CSTT'"));
  }
  strcpy_P(txcmd, PSTR("AT+CIICR\r"));
  if (!HandleAtCmd(txcmd, AT_OK)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CIICR'"));
    goto retry;
  }
  strcpy_P(txcmd, PSTR("AT+CIFSR\r"));
  if (!HandleAtCmd(txcmd, AT_IP)) {
    debugUart.sendStr_P(PSTR("\r\nERROR: 'AT+CIFSR'"));
    goto retry;
  }
  debugUart.sendStr_P(PSTR("\r\nDONE: TCP"));
  return true;
}
/*******************************************************************************/
bool Cmodem::connect(void) {
  c08 txcmd[bufSize];
  strcpy(txcmd, "AT+CIPSTART=\"TCP\",\"");
  strcat(txcmd, serverIP);
  strcat(txcmd, "\",\"");
  strcat(txcmd, port);
  strcat(txcmd, "\"\r");
  if (!HandleAtCmd(txcmd, AT_OK))
    return false;
  ss = SOCK_ESTABLISHED;
  return true;
}
/*******************************************************************************/
bool Cmodem::disconnect(void) {
  c08 txcmd[bufSize];
  strcpy(txcmd, "AT+CIPCLOSE\r");
  ss = SOCK_CLOSED;
  if (!HandleAtCmd(txcmd, AT_IP_CLOSE))
    return false;
  return true;
}

/*******************************************************************************/
void Cmodem::ServerSetIP(c08* _IP, c08 *_port, bool _usedns) {
  usedns = _usedns;
  strcpy(serverIP, _IP);
  strcpy(port, _port);
}

/*******************************************************************************/
bool Cmodem::write(u08* dat, u16 len) {
  c08 txcmd[bufSize];
  if (ss == SOCK_ESTABLISHED) {
    if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
      pUart->write((c08*) dat, len);
      pUart->sendStr_P(PSTR("\x1A\r"));
      return true;
    }
  }
  return false;
}
/*******************************************************************************/
bool Cmodem::write(Tfifo<u08>* dat) {
  if (ss == SOCK_ESTABLISHED) {
    pUart->clear();
    if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
      pUart->write(dat);
      pUart->sendStr_P(PSTR("\x1A\r"));
      return true;
    }
  }
  return false;
}
/*******************************************************************************/
void Cmodem::service(void) {
  c08* pstr = 0;
  c08* start = 0;
  c08* end = 0;
  u08 len, lenF;
  c08 strLen[4];
  u08 buf[bufSize];

  if (isSet()) {
    memset(gprsraw, 0, bufSize);
    lenF = pUart->rxFifo.read(gprsraw, 0, true);
    if (lenF) {
      pstr = strstr_P((c08*) gprsraw, PSTR("+IPD"));
      if (pstr != 0) {
        start = strchr((c08*) pstr, 'D');
        end = strchr((c08*) pstr, ':');
        if (start && end) {
          strncpy(strLen, start + 1, end - start - 1);
          len = atoi(strLen);
          if ((lenF - (end - (c08*) gprsraw + 1)) >= len) {
            memcpy(buf, end + 1, len);
            rxFifo.write(buf, len);
            pUart->clearRx();
          }
        }
      }
    }
  }
}
/*******************************************************************************/
bool Cmodem::PowerOff(void) {
  MDM_PWR_KEY_ON();
  _delay_ms(2000);
  MDM_PWR_KEY_OFF();
  _delay_ms(2000);
  MDM_PWR_FET_OFF();
  _delay_ms(500);
  return true;
}

/*******************************************************************************/
bool Cmodem::PowerOn(void) {
  MDM_PWR_FET_ON();
  _delay_ms(200);
  MDM_PWR_KEY_ON();
  _delay_ms(2000);
  MDM_PWR_KEY_OFF();
  _delay_ms(2000);
  return true;
}
/*******************************************************************************/

