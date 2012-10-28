#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "common.h"
#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "CUART.h"
#include "CTimer.h"
#include "CModem.h"
#include "iopins.h"
#include "main.h"

#define DYNDNS_IP   "204.13.248.74"

//#define SMS_DEBUG

/*
 AT = OK, just to make the modem respond
 ATE1 = OK , enable the ECHO
 AT+CGATT=1 = OK, enable GPRS
 AT+CGDCONT=1,"IP","here_apn_provider" = OK, change for your APN
 AT+CSTT="here_apn_provider","user","pass" = Ok
 AT+CDNSORIP=0 = OK = to accept IP address(No domain name)
 AT+CIICR = OK, connect to the gprs using the apn
 AT+CIFSR = to check the IP address the provider give you
 AT+CLPORT="TCP","1720" = IF you using TCP, for UDP skip this one
 AT+CIPSTART="TCP","192.168.1.1","1720" = use your IP address and port it will respond CONNECT OK

 AT+CIPSTART="UDP","192.168.1.1","1720" = this is for UDP,
 */

PROGMEM char AT_OK[] = "\r\nOK\r\n";
PROGMEM char AT_RDY[] = ">";
PROGMEM char AT_NONE[] = "";
PROGMEM char AT_SEND_OK[] = "SEND OK";
PROGMEM char AT_CONNECT[] = "CONNECT OK";
PROGMEM char AT_ERROR[] = "ERROR";
PROGMEM char AT_DATA[] = "\r\n";
PROGMEM char SIM_RDY[] = "+CPIN: READY";

extern CUART DbgUart;
/*******************************************************************************/
CModem::CModem(CUART * _pUart) {
  usedns = false;
  pUart = _pUart;
  retry = MDM_RETRIES_VAL;
  taskRetry = MDM_RETRIES_VAL;
  timeout = MDM_TIMEOUT_VAL;
  mdmState = MDM_STARTUP;
  cmdState = START;
  signal_ok = false;
  simcard_ok = false;
  connect_ok = false;
  registered_ok = false;
  pwrState = 1;
  initState = 1;
  strcpy(serverIP, "");
  strcpy(port, "");
  serviceTime = 0;
  atomicTime = 0;
  failState = MDM_STARTUP;
  error_cnt = 0;
  smsrx = false;
  smstx_en = true;
  gprsrx = false;
}
/*******************************************************************************/
bool CModem::Init(void) {
  c08* str;
  c08* pStr;
  c08 rxmsg[MDM_MAX_RX_CMD_LEN];
  u16 cnt = 0;
  bool done = false;
  retry: PowerOff();
  PowerOn();
  CLR_DTR();
  CLR_RTS();
  _delay_ms(200);
  SET_DTR();
  _delay_ms(200);
  //Put RTS_logic 0 --> ready uP ready to TX
  SET_RTS();
  while (GET_CTS_STATUS()) {
    _delay_ms(5);
  }
  _delay_ms(6000);
  // Check Call Ready
//  pUart->clearRx();
//  while(1){
//    cnt++;
//  }
//  memset(rxmsg, 0, sizeof(rxmsg));
//  str = rxmsg;
//  do {
//    if (pUart->rxnum() > 0) {
//      pUart->receive((u08*) str, 1);
//      str++;
//      if (strstr_P(rxmsg, PSTR("Call Ready"))) {
//        done = true;
//      }
//    }
//    _delay_ms(5);
//    cnt++;
//    if (cnt > 500) {
//      //goto retry;
//    }
//  } while (!done);
//  while (1) {
//    if (DbgUart.receive((u08*) rxmsg, 1) == 1) {
//      pUart->send((c08*) rxmsg, 1);
//    }
//    if (pUart->receive((u08*) rxmsg, 1) == 1) {
//      DbgUart.send(rxmsg, 1);
//    }
//  }
  HandleAtCmd("ATE0\r", AT_NONE);
  HandleAtCmd(PSTR("ATE0\r"), AT_NONE);
  HandleAtCmd(PSTR("ATE0\r"), AT_NONE);
  HandleAtCmd(PSTR("ATE0\r"), AT_NONE);
  if (!HandleAtCmd(PSTR("AT\r"), AT_OK))
    goto retry;
//  if (!HandleAtCmd(PSTR("AT+CIPHEAD=1\r"), AT_OK))
//    goto retry;
//  if (!HandleAtCmd(PSTR("AT+CMGF=1\r"), AT_OK))
//    goto retry;
//  if (HandleAtCmd(PSTR("AT+CSQ\r"), AT_OK, rxmsg)) {
//    DbgUart.sendStr(rxmsg);
//    pStr = rxmsg;
//    str = strsep(&pStr, ":");
//    DbgUart.sendStr(str);
//    str = strsep(&pStr, ",");
//    DbgUart.sendStr(str);
//    signalStrenght = atoi(str);
//    signal_ok = false;
//    if (signalStrenght < 30 && signalStrenght > 5) {
//      signal_ok = true;
//    } else {
//      DbgUart.sendStr_P(PSTR("\n\rSIGNAL STRENGHT TOO LOW!!"));
//    }
//  } else {
//    goto retry;
//  }
//  if (!HandleAtCmd(PSTR("AT+CPIN?\r"), AT_OK, rxmsg)) {
//    goto retry;
//  } else {
//    simcard_ok = false;
//    if (strstr_P(rxmsg, PSTR("+CPIN: READY"))) {
//      simcard_ok = true;
//    } else if (strstr_P(rxmsg, PSTR("+CME ERROR"))) {
//      DbgUart.sendStr_P(PSTR("\n\rNO SIMCARD!!"));
//      error = ERR_NO_SIMCARD;
//      return false;
//    } else if (strstr_P(rxmsg, PSTR("+CPIN: SIM PIN"))) {
//      DbgUart.sendStr_P(PSTR("\n\rPIN ON SIMCARD!!"));
//      error = ERR_PIN_ON_SIMCARD;
//      return false;
//    }
//  }
//  }
//  break;
//case MDM_SET_SIMCARD_PIN:
//  timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//  strcpy_P(txcmd, PSTR("AT+CPIN="));
//  strcpy(pincode, "3470");
//  if (strlen(pincode) < 4) {
//    mdmState = MDM_FAILED; // TODO maybe goto a restart state?
//#if MDM_DEBUG_LEVEL > 2
//        DbgUart.sendStr_P(PSTR("\n\rNO VALID PIN SET"));
//#endif
//    break;
//  }
//  strcat(txcmd, pincode);
//  strcat_P(txcmd, PSTR("\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    simcard_ok = true;
//    mdmState = MDM_GET_SIMCARD_STATUS;
//  }
//  break;

  if (!HandleAtCmd(PSTR("AT+CREG?\r"), AT_OK))
    goto retry;
//  registered_ok = false;
//  if (strstr_P(rxmsg, PSTR("+CREG: 0,1"))) {
//    registered_ok = true;
//    mdmState = MDM_IP_STATUS;
//  } else {
//    error_cnt++;
//    if (error_cnt > 200) {
//      mdmState = MDM_FAILED;
//      DbgUart.sendStr_P(PSTR("\n\rSIMCARD REGISTRATION FAILED!!"));
//    }
//  }
//  strcpy_P(txcmd, PSTR("AT+CIPSTATUS\r"));
//  connect_ok = false;
//  if (HandleAtCmd(PSTR("AT+CIPSTATUS\r"), AT_DATA)) {
//    if (strstr_P(rxmsg, AT_CONNECT)) {
//      mdmState = MDM_READY;
//      connect_ok = true;
//      DbgUart.sendStr((u08*) "MODEM CONNECTED!!");
//    }
//    if (strstr_P(rxmsg, PSTR("INITIAL"))) {
//      mdmState = MDM_CONNECT_TO_SERVER;
//    } else if (strstr_P(rxmsg, PSTR("IP CLOSE"))) {
//      mdmState = MDM_START_IP;
//    } else if (strstr_P(rxmsg, PSTR("STATE: PDP DEACT"))) {
//      mdmState = MDM_IPSHUTDOWN;
//    } else {
//      mdmState = MDM_READY;
//    }
//  }
//  break;
//
//  /*******************************************************************************/
//  case MDM_READ_SMS:
//  strcpy_P(txcmd, PSTR("AT+CMGR="));
//  strcat(txcmd, sms.nr);
//  strcat_P(txcmd, PSTR("\r"));
//  if (HandleAtCmd(txcmd, AT_DATA)) {
//    strcpy(sms.message, rxmsg);
//    mdmState = MDM_READY;
//    smsrx = true;
//  }
//  break;
//
//  case MDM_SEND_SMS:
//  timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//#ifdef SMS_DEBUG
//#warning SMSDEBUG IS ENABLED
//      if(atomicTime>timeout) {
//        mdmState = MDM_READY;
//        DbgUart.sendStr_P(PSTR("\n\r ## SMS SEND OK ##"));
//      }
//#else
//
//  strcpy_P(txcmd, PSTR("AT+CMGS=\""));
//  strcat(txcmd, sms.phonenum);
//  strcat_P(txcmd, PSTR("\"\r"));
//  if (HandleAtCmd(txcmd, AT_RDY)) {
//    pUart->sendStr((u08*) sms.message);
//    pUart->sendStr_P(PSTR("\x1A"));
//    mdmState = MDM_SMS_WAIT_SEND_OK;
//  } else if (cmdState == SEND) { //a timeout occured
//    pUart->sendStr_P(PSTR("\x1B")); // send an escape
//  }
//  break;
//
//  case MDM_SMS_WAIT_SEND_OK:
//  timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//  if (GetAtResp(AT_OK)) {
//#if MDM_DEBUG_LEVEL > 2
//    DbgUart.sendStr_P(PSTR("\n\rSMS SEND OK"));
//#endif
//    mdmState = MDM_READY;
//  } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//      DbgUart.sendStr_P(PSTR("\n\rSMS SEND ERROR"));
//#endif
//    mdmState = MDM_SEND_SMS;
//    taskRetry++;
//  }
//#endif
//  break;
//
//  case MDM_CONNECT_TO_SERVER:
//  strcpy_P(txcmd, PSTR("AT+CGDCONT=1,\"IP\",\"internet\"\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_SET_SHOW_REMOTE_IP;
//  }
//  break;
//  case MDM_SET_SHOW_REMOTE_IP:
//  strcpy_P(txcmd, PSTR("AT+CIPSRIP=0\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_SET_DNS_CONNECT;
//  }
//  break;
//  case MDM_SET_DNS_CONNECT:
//  failState = MDM_IPSHUTDOWN;
//  if (usedns) {
//    strcpy_P(txcmd, PSTR("AT+CDNSORIP=1\r"));
//    if (HandleAtCmd(txcmd, AT_OK)) {
//      mdmState = MDM_SET_DNS_CONFIG;
//    }
//  } else {
//    strcpy_P(txcmd, PSTR("AT+CDNSORIP=0\r"));
//    if (HandleAtCmd(txcmd, AT_OK)) {
//      mdmState = MDM_SET_TCP_PORT;
//    }
//  }
//  break;
//  case MDM_SET_DNS_CONFIG:
//  failState = MDM_IPSHUTDOWN;
//  strcpy_P(txcmd, PSTR("AT+CDNSCFG=\""));
//  strcat(txcmd, DYNDNS_IP);
//  strcat_P(txcmd, PSTR("\"\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_SET_TCP_PORT;
//  }
//  break;
//  case MDM_SET_TCP_PORT:
//  failState = MDM_IPSHUTDOWN;
//
//  strcpy_P(txcmd, PSTR("AT+CLPORT=\"TCP\",\""));
//  strcat(txcmd, port);
//  strcat(txcmd, "\"\r");
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_START_TCP_TASK;
//  }
//  break;
//  case MDM_START_TCP_TASK:
//  failState = MDM_IPSHUTDOWN;
//  timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//  strcpy_P(txcmd, PSTR("AT+CSTT=\"internet\",\"\",\"\"\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_ACTIVATE_PDP;
//  }
//  break;
//  case MDM_ACTIVATE_PDP:
//  failState = MDM_IPSHUTDOWN;
//  timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//  strcpy_P(txcmd, PSTR("AT+CIICR\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_GET_IP;
//  }
//  break;
//  case MDM_GET_IP:
//  failState = MDM_IPSHUTDOWN;
//  timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//  strcpy_P(txcmd, PSTR("AT+CIFSR\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_START_IP;
//  }
//  if (cmdState == SEND) {
//    DbgUart.sendStr((u08*) rxmsg);
//    cmdState = START;
//    mdmState = MDM_START_IP;
//  }
//  break;
//  case MDM_START_IP:
//  timeout = MDM_TIMEOUT_VAL * 30; //20 sekondes
//  failState = MDM_IPSHUTDOWN;
//  strcpy(txcmd, "AT+CIPSTART=\"TCP\",\"");
//  strcat(txcmd, serverIP);
//  strcat(txcmd, "\",\"");
//  strcat(txcmd, port);
//  strcat(txcmd, "\"\r");
//  connect_ok = false;
//  if (HandleAtCmd(txcmd, AT_CONNECT)) {
//    connect_ok = true;
//    DbgUart.sendStr((u08*) "MODEM CONNECTED!!");
//    mdmState = MDM_IP_STATUS;
//  }
//  if (cmdState == SEND) {
//    cmdState = START;
//    mdmState = MDM_IP_STATUS;
//  }
//  break;
//  case MDM_IP_SEND:
//  failState = MDM_START_IP;
//  if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
//    mdmState = MDM_IPSEND_VERIFY;
//    cmdState = START;
//    DbgUart.sendStr((u08*) txcmd);
//    pUart->sendStr((u08*) txcmd);
//    pUart->sendStr_P(PSTR("\x1A\r"));
//  }
//  break;
//  case MDM_IPSEND_VERIFY:
//  timeout = MDM_TIMEOUT_VAL * 30;
//  failState = MDM_IPSHUTDOWN;
//  if (GetAtResp(AT_SEND_OK)) {
//    DbgUart.sendStr((u08*) "\n\rSUKSES");
//    mdmState = MDM_READY;
//  } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//      DbgUart.sendStr_P(PSTR("\n\rDATA SEND ERROR"));
//#endif
//    mdmState = MDM_IP_SEND;
//    taskRetry++;
//  }
//  break;
//
//  case MDM_IPSHUTDOWN:
//  strcpy_P(txcmd, PSTR("AT+CIPSHUT\r"));
//  if (HandleAtCmd(txcmd, AT_OK)) {
//    mdmState = MDM_CONNECT_TO_SERVER;
//  }
//  connect_ok = false;
//  break;
//
//  case MDM_GET_BALANCE:
//  timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//  strcpy_P(txcmd, PSTR("ATD*100#\r"));
//  if (HandleAtCmd(txcmd, AT_DATA)) {
//    mdmState = MDM_READY;
//    DbgUart.sendStr((u08*) rxmsg);
//  }
//  break;
//
//  case MDM_FAILED:
//  break;
//
//  case MDM_READY:
//  break;
//
//  default:
//  break;
//}
//
//return true;
//}
//
//void CModem::clearTimer(void) {
//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
//{
//  isr_timer = 0;
//  atomicTime = 0;
//}
  return true;
}

/*******************************************************************************/
void CModem::ServerSetIP(c08* _IP, c08 *_port, bool _usedns) {
  usedns = _usedns;
  strcpy(serverIP, _IP);
  strcpy(port, _port);
}

/*******************************************************************************/
bool CModem::DataToServer(c08* dat) {
  if (mdmState == MDM_READY&& connect_ok == true && registered_ok == true && simcard_ok
  == true && signal_ok == true) {
    strcpy(txcmd, dat);
    mdmState = MDM_IP_SEND;
    return true;
  } else {
    return false;
  }
}
/*******************************************************************************/
bool CModem::SendSMS(char *PhoneNumber, char *Message) {
  if (mdmState == MDM_READY && simcard_ok == true && smstx_en == true) {
    mdmState = MDM_SEND_SMS;
    strcpy(sms.phonenum, PhoneNumber);
    strcpy(sms.message, Message);
    return true;
  } else {
    return false;
  }
}
/*******************************************************************************/
bool CModem::GetSignalQuality(void) {
  if (mdmState == MDM_READY) {
    mdmState = MDM_GET_SIGNAL_STRENGHT;
    return true;
  } else {
    return false;
  }
}
/*******************************************************************************/
bool CModem::SIMCheckReady(void) {
  if (mdmState == MDM_READY) {
    mdmState = MDM_GET_SIMCARD_STATUS;
    return true;
  } else {
    return false;
  }
}

void CModem::UpdateMdmStatus(void) {
  if (mdmState == MDM_READY) {
    mdmState = MDM_GET_SIMCARD_STATUS;
  }
}

/*
 bool CModem::SIMSetPin(c08* pin) {
 if (mdmState == MDM_READY) {
 strcpy(pincode, pin);
 mdmState = MDM_SET_SIMCARD_PIN;
 return true;
 } else {
 return false;
 }
 }
 */

/*******************************************************************************/
eMdmState CModem::GetStateModem(void) {
  return mdmState;
}
/******************************************************************************/
void CModem::setNextState(eMdmState nextState) {
  //if (mdmState == MDM_READY) { // HACK
  mdmState = nextState;
  //}
}
///*******************************************************************************/
//bool CModem::Init(void) {
//  u08 ret;
//  ret = false;
//  switch (initState) {
//    case 1:
//      if (PowerOff())
//        initState = 2;
//      break;
//    case 2:
//      if (PowerOn()) {
//        CLR_DTR();
//        CLR_RTS();
//        initState = 3;
//        clearTimer();
//      }
//      break;
//    case 3:
//      if (atomicTime > 200000) {
//        clearTimer();
//        // uP tell modem its ready to Communicate
//        SET_DTR();
//        initState = 4;
//      }
//      break;
//    case 4:
//      if (atomicTime > 200000) {
//        clearTimer();
//        //Put RTS_logic 0 --> ready uP ready to TX
//        SET_RTS();
//        initState = 5;
//      }
//      break;
//    case 5:
//      // Wait for Modem (DCE) to respond with a cts logic 0 - clear to send
//      if (!GET_CTS_STATUS()) {
//        clearTimer();
//        initState = 6;
//      }
//      if (atomicTime > 1000000) {
//        clearTimer();
//        initState = 1;
//      }
//      if (retry > 10) {
//        mdmState = failState;
//        initState = 1;
//        cmdState = START;
//      }
//      break;
//
//    case 6:
//      if (atomicTime > 5000000) { // wait 5 seconds form modem to register
//        clearTimer();
//        cmdState = START;
//        initState = 1;
//        ret = true;
//      }
//      break;
//  }
//  return ret;
//}

/*******************************************************************************/
//void CModem::Service(void) {
//  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
//  {
//    atomicTime = isr_timer;
//    serviceTime = isr_timer;
//  }
//
//  if (serviceTime < SERVICE_INTERVAL) {
//    return;
//  }
//  serviceTime = 0;
//
//  timeout = MDM_TIMEOUT_VAL;
//  GetUnSolicited();
//
//  if (taskRetry > MDM_RETRIES_VAL) {
//    taskRetry = 0;
//    mdmState = MDM_READY;
//  }
//  failState = MDM_STARTUP;
//
//  switch (mdmState) {
//    /*******************************************************************************/
//    case MDM_STARTUP:
//      switch (cmdState) {
//        case START:
//          retry = 0;
//          DbgUart.sendStr_P(PSTR("\n\rMDM_STARTUP"));
//          cmdState = RETRY;
//          initState = 1;
//          signal_ok = false;
//          simcard_ok = false;
//          connect_ok = false;
//          registered_ok = false;
//          break;
//        case RETRY:
//        case SEND:
//          if (Init()) {
//            mdmState = MDM_CONFIGURE;
//          }
//          break;
//      }
//      break;
//    case MDM_CONFIGURE:
//      strcpy_P(txcmd, PSTR("ATE0\r"));
//      if (HandleAtCmd(txcmd, AT_NONE)) {
//        mdmState = MDM_TEST_AT;
//      }
//      break;
//    case MDM_TEST_AT:
//      strcpy_P(txcmd, PSTR("AT\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_ADD_IPHEAD;
//      }
//      break;
//    case MDM_GET_TIME:
//      strcpy_P(txcmd, PSTR("AT+CCLK\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_READY;
//      }
//      break;
//    case MDM_ADD_IPHEAD:
//      strcpy_P(txcmd, PSTR("AT+CIPHEAD=1\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_SMS_TEXT_MODE;
//      }
//      break;
//    case MDM_SET_SMS_TEXT_MODE:
//      strcpy_P(txcmd, PSTR("AT+CMGF=1\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_GET_SIGNAL_STRENGHT;
//        error_cnt = 0;
//      }
//      break;
//    case MDM_GET_SIGNAL_STRENGHT:
//      strcpy_P(txcmd, PSTR("AT+CSQ\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        pRx = rxmsg;
//        str = strsep(&pRx, ":");
//        DbgUart.sendStr((u08*) str);
//        str = strsep(&pRx, ",");
//        DbgUart.sendStr((u08*) str);
//        signalStrenght = atoi(str);
//        signal_ok = false;
//        if (signalStrenght < 30 && signalStrenght > 5) {
//          signal_ok = true;
//          mdmState = MDM_GET_SIMCARD_STATUS;
//        } else {
//          error_cnt++;
//          if (error_cnt > 200) {
//            mdmState = MDM_FAILED;
//            DbgUart.sendStr_P(PSTR("\n\rSIGNAL STRENGHT TOO LOW!!"));
//          }
//        }
//      }
//      break;
//    case MDM_GET_SIMCARD_STATUS:
//      strcpy_P(txcmd, PSTR("AT+CPIN?\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        simcard_ok = false;
//        if (strstr_P(rxmsg, PSTR("+CPIN: READY"))) {
//          simcard_ok = true;
//          mdmState = MDM_IS_REGISTERED;
//          error_cnt = 0;
//        } else if (strstr_P(rxmsg, PSTR("+CPIN: SIM PIN"))) {
//          mdmState = MDM_SET_SIMCARD_PIN;
//          error_cnt = 0;
//        }
//      }
//      if (strstr_P(rxmsg, PSTR("+CME ERROR"))) {
//        mdmState = MDM_FAILED;
//        DbgUart.sendStr_P(PSTR("\n\rNO SIMCARD!!"));
//      }
//      break;
//    case MDM_SET_SIMCARD_PIN:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//      strcpy_P(txcmd, PSTR("AT+CPIN="));
//      strcpy(pincode, "3470");
//      if (strlen(pincode) < 4) {
//        mdmState = MDM_FAILED; // TODO maybe goto a restart state?
//#if MDM_DEBUG_LEVEL > 2
//            DbgUart.sendStr_P(PSTR("\n\rNO VALID PIN SET"));
//#endif
//        break;
//      }
//      strcat(txcmd, pincode);
//      strcat_P(txcmd, PSTR("\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        simcard_ok = true;
//        mdmState = MDM_GET_SIMCARD_STATUS;
//      }
//      break;
//
//    case MDM_IS_REGISTERED:
//      strcpy_P(txcmd, PSTR("AT+CREG?\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        registered_ok = false;
//        if (strstr_P(rxmsg, PSTR("+CREG: 0,1"))) {
//          registered_ok = true;
//          mdmState = MDM_IP_STATUS;
//        } else {
//          error_cnt++;
//          if (error_cnt > 200) {
//            mdmState = MDM_FAILED;
//            DbgUart.sendStr_P(PSTR("\n\rSIMCARD REGISTRATION FAILED!!"));
//          }
//        }
//      }
//      break;
//
//    case MDM_IP_STATUS:
//      strcpy_P(txcmd, PSTR("AT+CIPSTATUS\r"));
//      connect_ok = false;
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        if (strstr_P(rxmsg, AT_CONNECT)) {
//          mdmState = MDM_READY;
//          connect_ok = true;
//          DbgUart.sendStr((u08*) "MODEM CONNECTED!!");
//        }
//        if (strstr_P(rxmsg, PSTR("INITIAL"))) {
//          mdmState = MDM_CONNECT_TO_SERVER;
//        } else if (strstr_P(rxmsg, PSTR("IP CLOSE"))) {
//          mdmState = MDM_START_IP;
//        } else if (strstr_P(rxmsg, PSTR("STATE: PDP DEACT"))) {
//          mdmState = MDM_IPSHUTDOWN;
//        } else {
//          mdmState = MDM_READY;
//        }
//      }
//      break;
//
//      /*******************************************************************************/
//    case MDM_READ_SMS:
//      strcpy_P(txcmd, PSTR("AT+CMGR="));
//      strcat(txcmd, sms.nr);
//      strcat_P(txcmd, PSTR("\r"));
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        strcpy(sms.message, rxmsg);
//        mdmState = MDM_READY;
//        smsrx = true;
//      }
//      break;
//
//    case MDM_SEND_SMS:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//#ifdef SMS_DEBUG
//#warning SMSDEBUG IS ENABLED
//          if(atomicTime>timeout) {
//            mdmState = MDM_READY;
//            DbgUart.sendStr_P(PSTR("\n\r ## SMS SEND OK ##"));
//          }
//#else
//
//      strcpy_P(txcmd, PSTR("AT+CMGS=\""));
//      strcat(txcmd, sms.phonenum);
//      strcat_P(txcmd, PSTR("\"\r"));
//      if (HandleAtCmd(txcmd, AT_RDY)) {
//        pUart->sendStr((u08*) sms.message);
//        pUart->sendStr_P(PSTR("\x1A"));
//        mdmState = MDM_SMS_WAIT_SEND_OK;
//      } else if (cmdState == SEND) { //a timeout occured
//        pUart->sendStr_P(PSTR("\x1B")); // send an escape
//      }
//      break;
//
//    case MDM_SMS_WAIT_SEND_OK:
//      timeout = MDM_TIMEOUT_VAL * 5; //5seconds
//      if (GetAtResp(AT_OK)) {
//#if MDM_DEBUG_LEVEL > 2
//        DbgUart.sendStr_P(PSTR("\n\rSMS SEND OK"));
//#endif
//        mdmState = MDM_READY;
//      } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//          DbgUart.sendStr_P(PSTR("\n\rSMS SEND ERROR"));
//#endif
//        mdmState = MDM_SEND_SMS;
//        taskRetry++;
//      }
//#endif
//      break;
//
//    case MDM_CONNECT_TO_SERVER:
//      strcpy_P(txcmd, PSTR("AT+CGDCONT=1,\"IP\",\"internet\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_SHOW_REMOTE_IP;
//      }
//      break;
//    case MDM_SET_SHOW_REMOTE_IP:
//      strcpy_P(txcmd, PSTR("AT+CIPSRIP=0\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_DNS_CONNECT;
//      }
//      break;
//    case MDM_SET_DNS_CONNECT:
//      failState = MDM_IPSHUTDOWN;
//      if (usedns) {
//        strcpy_P(txcmd, PSTR("AT+CDNSORIP=1\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_DNS_CONFIG;
//        }
//      } else {
//        strcpy_P(txcmd, PSTR("AT+CDNSORIP=0\r"));
//        if (HandleAtCmd(txcmd, AT_OK)) {
//          mdmState = MDM_SET_TCP_PORT;
//        }
//      }
//      break;
//    case MDM_SET_DNS_CONFIG:
//      failState = MDM_IPSHUTDOWN;
//      strcpy_P(txcmd, PSTR("AT+CDNSCFG=\""));
//      strcat(txcmd, DYNDNS_IP);
//      strcat_P(txcmd, PSTR("\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_SET_TCP_PORT;
//      }
//      break;
//    case MDM_SET_TCP_PORT:
//      failState = MDM_IPSHUTDOWN;
//
//      strcpy_P(txcmd, PSTR("AT+CLPORT=\"TCP\",\""));
//      strcat(txcmd, port);
//      strcat(txcmd, "\"\r");
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_START_TCP_TASK;
//      }
//      break;
//    case MDM_START_TCP_TASK:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//      strcpy_P(txcmd, PSTR("AT+CSTT=\"internet\",\"\",\"\"\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_ACTIVATE_PDP;
//      }
//      break;
//    case MDM_ACTIVATE_PDP:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 30; //30 sekondes
//      strcpy_P(txcmd, PSTR("AT+CIICR\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_GET_IP;
//      }
//      break;
//    case MDM_GET_IP:
//      failState = MDM_IPSHUTDOWN;
//      timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//      strcpy_P(txcmd, PSTR("AT+CIFSR\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_START_IP;
//      }
//      if (cmdState == SEND) {
//        DbgUart.sendStr((u08*) rxmsg);
//        cmdState = START;
//        mdmState = MDM_START_IP;
//      }
//      break;
//    case MDM_START_IP:
//      timeout = MDM_TIMEOUT_VAL * 30; //20 sekondes
//      failState = MDM_IPSHUTDOWN;
//      strcpy(txcmd, "AT+CIPSTART=\"TCP\",\"");
//      strcat(txcmd, serverIP);
//      strcat(txcmd, "\",\"");
//      strcat(txcmd, port);
//      strcat(txcmd, "\"\r");
//      connect_ok = false;
//      if (HandleAtCmd(txcmd, AT_CONNECT)) {
//        connect_ok = true;
//        DbgUart.sendStr((u08*) "MODEM CONNECTED!!");
//        mdmState = MDM_IP_STATUS;
//      }
//      if (cmdState == SEND) {
//        cmdState = START;
//        mdmState = MDM_IP_STATUS;
//      }
//      break;
//    case MDM_IP_SEND:
//      failState = MDM_START_IP;
//      if (HandleAtCmd("AT+CIPSEND\r", AT_RDY)) {
//        mdmState = MDM_IPSEND_VERIFY;
//        cmdState = START;
//        DbgUart.sendStr((u08*) txcmd);
//        pUart->sendStr((u08*) txcmd);
//        pUart->sendStr_P(PSTR("\x1A\r"));
//      }
//      break;
//    case MDM_IPSEND_VERIFY:
//      timeout = MDM_TIMEOUT_VAL * 30;
//      failState = MDM_IPSHUTDOWN;
//      if (GetAtResp(AT_SEND_OK)) {
//        DbgUart.sendStr((u08*) "\n\rSUKSES");
//        mdmState = MDM_READY;
//      } else if (cmdState == SEND) { //a timeout occured
//#if MDM_DEBUG_LEVEL > 2
//          DbgUart.sendStr_P(PSTR("\n\rDATA SEND ERROR"));
//#endif
//        mdmState = MDM_IP_SEND;
//        taskRetry++;
//      }
//      break;
//
//    case MDM_IPSHUTDOWN:
//      strcpy_P(txcmd, PSTR("AT+CIPSHUT\r"));
//      if (HandleAtCmd(txcmd, AT_OK)) {
//        mdmState = MDM_CONNECT_TO_SERVER;
//      }
//      connect_ok = false;
//      break;
//
//    case MDM_GET_BALANCE:
//      timeout = MDM_TIMEOUT_VAL * 10; //10 sekondes
//      strcpy_P(txcmd, PSTR("ATD*100#\r"));
//      if (HandleAtCmd(txcmd, AT_DATA)) {
//        mdmState = MDM_READY;
//        DbgUart.sendStr((u08*) rxmsg);
//      }
//      break;
//
//    case MDM_FAILED:
//      break;
//
//    case MDM_READY:
//      break;
//
//    default:
//      break;
//  }
//}
/*******************************************************************************/
bool CModem::HandleAtCmd(const char *cmd, char* rspStr, c08* _rxRsp) {
  bool ret;
  strcpy_P(txcmd, cmd);
  DbgUart.sendStr(txcmd);
  pUart->sendStr(txcmd);
  ret = GetAtResp(rspStr, _rxRsp);
  return ret;
}
/*******************************************************************************/
bool CModem::GetAtResp(char* _expRsp, c08* _rxRsp) {
  char expRsp[MDM_MAX_RX_CMD_LEN];
  char rxRsp[MDM_MAX_RX_CMD_LEN];
  memset(expRsp, 0, MDM_MAX_RX_CMD_LEN);
  memset(rxRsp, 0, MDM_MAX_RX_CMD_LEN);
  u08 cnt = 0;
  u08 len;
  strcpy_P((c08*) expRsp, _expRsp);
  len = strlen((c08*) expRsp);
  while (pUart->rxnum() < len) {
    _delay_ms(5);
    if ((cnt++) > 200)
      return false;
  }
  len = pUart->rxnum();
  pUart->receive((u08*) rxRsp, len);
  DbgUart.sendStr(rxRsp);
  if (_rxRsp) {
    strcpy((char*) _rxRsp, rxRsp);
  }
  if (strstr((const char*) rxRsp, (c08*) expRsp)) {
    return true;
  }
  return false;
}
/*******************************************************************************/
//void CModem::GetUnSolicited(void) {
//  char *pstr = 0;
//  char *start = 0;
//  char *end = 0;
//  u08 len;
//  if (pUart->peek(rxmsg)) {
//
//    if (strstr_P(rxmsg, PSTR("CONNECT FAIL"))) {
//      mdmState = MDM_IPSHUTDOWN;
//    }
//    if (strstr_P(rxmsg, PSTR("+CMTI: \"SM\","))) {
//      mdmState = MDM_READ_SMS;
//      pstr = strchr(rxmsg, ',');
//      if (pstr) {
//        pstr++;
//        if (strlen(pstr) > 10) {
//          pstr[9] = 0;
//          strncpy(sms.nr, pstr, 10);
//        } else {
//          strcpy(sms.nr, pstr);
//        }
//      } else {
//        strcpy(sms.nr, "0");
//      }
//      pUart->clearRx();
//    }
//    if (strstr_P(rxmsg, PSTR("+CME"))) {
//      mdmState = MDM_READY;
//      pUart->clearRx();
//    }
//    pstr = strstr_P(rxmsg, PSTR("+IPD"));
//    if (pstr != 0 && gprsrx == false) {
////DbgUart.sendStr(rxmsg);
//      start = strchr(rxmsg, '*');
//      if (start) {
//        end = strchr(rxmsg, '#');
//        if (end) {
//          //CLR_RTS();
//          len = end - start;
//          memcpy(gprsraw, start, len);
//          DbgUart.sendStr_P(PSTR("\n\r ==============RECEIVED STRING=============\n\r"));
//          DbgUart.sendStr((u08*) gprsraw);
//          //pUart->receive((u08*)NULL,(end-pstr));
//          DbgUart.sendStr_P(PSTR("\n\r=============== END =======================\n\r"));
//
//          gprsrx = true;
//          mdmState = MDM_READY;
//          pUart->clearRx();
//        }
//      }
//    }
//    //SET_RTS();
//  }
//}
/*******************************************************************************/
bool CModem::PowerOff(void) {
  MDM_PWR_KEY_ON();
  _delay_ms(2000);
  MDM_PWR_KEY_OFF();
  _delay_ms(2000);
  MDM_PWR_FET_OFF();
  _delay_ms(500);
  return true;
}

/*******************************************************************************/
bool CModem::PowerOn(void) {
  MDM_PWR_FET_ON();
  _delay_ms(200);
  MDM_PWR_KEY_ON();
  _delay_ms(2000);
  MDM_PWR_KEY_OFF();
  _delay_ms(2000);
  return true;
}
/*******************************************************************************/
