/****************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
/****************************************************************************************/
#include "uart.h"
#include "iopins.h"
#include "crc.h"
#include "types.h"
#include "server.h"
/****************************************************************************************/
#undef DEBUG
/****************************************************************************************/
Cserver::Cserver(Cserial* _serial, u08 _size, u08 _node) {
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
void Cserver::setPayloadBufSize(u08 size) {
  while (payload == NULL) {
    payload = (u08*) malloc(size);
  }
}
/****************************************************************************************/
void Cserver::service(void) {
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
        serial->read((u08*) &Header.Length, 4);
        htonl(&Header.Length);
        if (Header.Length <= MAX_PACKET_LEN) {
          State = STATE_RX_MSG_ID;
        }
      }
      break;
    case STATE_RX_MSG_ID:
      if (serial->read(&Header.MsgID, 1) == 1) {
        cntByte = 1;
        State = STATE_RX_PAYLOAD;
      }
      break;
    case STATE_RX_PAYLOAD:
      if (serial->read(&payload[cntByte], 1) == 1) {
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
void Cserver::reset(void) {
  State = STATE_RX_LENGTH;
}
/****************************************************************************************/
u08 Cserver::packetAvailable(void) {
  if (State == STATE_PACKET_AVAILABLE) {
    return true;
  }
  return false;
}
/****************************************************************************************/
void Cserver::statusUpdate(u08* strID, u08* strGPS, u08* strRFID, eCoverStatus* status) {
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
  serial->write((u08*) &Header, sizeof(Header));
// E_UNIT_ID
  strHdr.elementID = E_UNIT_ID;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strID);
  serial->write((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strID);
// E_UNIT_GPS
  strHdr.elementID = E_UNIT_GPS;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strGPS);
  serial->write((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strGPS);
// E_COVER_RFID
  strHdr.elementID = E_COVER_RFID;
  strHdr.type = T_STRING;
  strHdr.strLen = strlen((c08*) strRFID);
  serial->write((u08*) &strHdr, sizeof(sDataStringHeader));
  serial->sendStr((c08*) strRFID);
// E_COVER_STATUS
  binHdr.elementID = E_COVER_STATUS;
  binHdr.type = T_UCHAR;
  serial->write((u08*) &binHdr, sizeof(sDataBinHeader));
  serial->write((u08*) status, 1);
}
/****************************************************************************************/
void Cserver::sendTestPkt() {
  u08 strID[32];
  u08 strGPS[32];
  u08 strRFID[32];
  eCoverStatus status;

  strcpy((c08*) &strID, "1234");
  strcpy((c08*) &strGPS, "1.2 3.4");
  strcpy((c08*) &strRFID, "5555");
  status = COVER_OPEN;

  statusUpdate(strID, strGPS, strRFID, &status);

}
