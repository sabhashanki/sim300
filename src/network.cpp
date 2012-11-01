/****************************************************************************************/
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include "iopins.h"
#else
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "main.h"
#define ATOMIC_BLOCK(val)
#endif
/****************************************************************************************/
#include <CUART.h>
#include "crc.h"
#include "avrlibtypes.h"
#include "network.h"
/****************************************************************************************/
#undef DEBUG
/****************************************************************************************/
#ifndef WIN32
#define EE_NODEID			(u08 *)(0x40)
#else
AnsiString str;
#endif
/****************************************************************************************/
CNetwork::CNetwork(Cserial* _serial, u08 _size, u08 _node) {
  this->serial = _serial;
  this->NodeId = _node;
  State = STATE_RX_LENGTH;
  time = 0;
  timeLimit = 0;
  payloadSize = _size;
  payload = NULL;
  healthy = true;
  baudRate = serial->getBaudRate();
  setPayloadBufSize(_size);
}
/****************************************************************************************/
void CNetwork::setPayloadBufSize(u08 size) {
  while (payload == NULL) {
    payload = (u08*) malloc(size);
  }
}
/****************************************************************************************/
void CNetwork::service(void) {
  serial->service();
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (time > timeLimit) {
      State = STATE_RX_LENGTH;
      time = 0;
    }
  }
  switch (State) {
    case STATE_RX_LENGTH:
      if (serial->rxnum() > 4) {
        serial->receive((u08*) &Header.Length, 4);
        htonl(&Header.Length);
        if (Header.Length <= MAX_PACKET_LEN) {
          State = STATE_RX_MSG_ID;
        }
      }
      break;
    case STATE_RX_MSG_ID:
      if (serial->receive(&Header.MsgID, 1) == 1) {
        cntByte = 1;
        State = STATE_RX_PAYLOAD;
      }
      break;
    case STATE_RX_PAYLOAD:
      if (serial->receive(&payload[cntByte], 1) == 1) {
        cntByte++;
        if (cntByte == Header.Length) {
          State = STATE_PACKET_AVAILABLE;
        }
      }
      break;
    case STATE_PACKET_AVAILABLE:
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        time = 0;
      }
      break;
    default:
      State = STATE_RX_LENGTH;
      break;
  }
}
/****************************************************************************************/
void CNetwork::reset(void) {
  State = STATE_RX_LENGTH;
}
/****************************************************************************************/
u08 CNetwork::packetAvailable(void) {
  if (State == STATE_PACKET_AVAILABLE) {
    return true;
  }
  return false;
}
/****************************************************************************************/
void CNetwork::statusUpdate(u08* strID, u08* strGPS, u08* strRFID, eCoverStatus* status) {
  sHeader Header;
  sDataStringHeader strHdr;
  sDataBinHeader binHdr;

  reset();
  serial->clearTx();
  serial->clearRx();
  Header.Length = 13 + strlen((c08*) strID) + strlen((c08*) strGPS)
      + strlen((c08*) strRFID);
  htonl(&Header.Length);
  Header.MsgID = U2S_STATUSUPDATE;
  serial->send((u08*) &Header, sizeof(Header));
// E_UNIT_ID
  strHdr.elementID = E_UNIT_ID;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strID);
  serial->send((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strID);
// E_UNIT_GPS
  strHdr.elementID = E_UNIT_GPS;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strGPS);
  serial->send((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strGPS);
// E_COVER_RFID
  strHdr.elementID = E_COVER_RFID;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strRFID);
  serial->send((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strRFID);
// E_COVER_STATUS
  binHdr.elementID = E_COVER_STATUS;
  binHdr.type = T_UCHAR;
  serial->send((u08*) &binHdr, sizeof(sDataBinHeader));
  serial->send((u08*) status, 1);
}
/****************************************************************************************/
void CNetwork::sendTestPkt() {
  u08 strID[32];
  u08 strGPS[32];
  u08 strRFID[32];
  eCoverStatus status;

  strcpy((c08*) &strID, "1234");
  strcpy((c08*) &strGPS, "1.2 3.4");
  strcpy((c08*) &strRFID, "X8974");
  status = COVER_OPEN;

  statusUpdate(strID, strGPS, strRFID, &status);

}
