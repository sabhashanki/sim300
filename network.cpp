/****************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
/****************************************************************************************/
#include "uart.h"
#include "crc.h"
#include "types.h"
#include "network.h"
/****************************************************************************************/
using namespace CNETWORK;
/****************************************************************************************/
Cnetwork::Cnetwork(Cuart* _uartNr, u08 _node, u08 _size) :
    Csignal(period) {
  this->uart = _uartNr;
  this->NodeId = _node;
  timeout.setPeriod(timeoutTime);
  State = STATE_RX_HEADER;
  payloadSize = 0;
  payload = 0;
  baudRate = this->uart->baudRate;
  setPayloadBufSize(_size);
  scheduler.attach(&timeout);
  scheduler.attach(this);
}
/****************************************************************************************/
void Cnetwork::setPayloadBufSize(u08 _size) {
  while (payload == NULL) {
    payload = (u08*) malloc(_size);
  }
  payloadSize = _size;
}
/****************************************************************************************/
void Cnetwork::service(void) {
  if (isSet()) {
    while (uart->rxnum()) {
      switch (State) {
        case STATE_RX_HEADER:
          if (uart->rxnum() >= sizeof(Header)) {
            uart->rxFifo.read((u08*) &Header, sizeof(Header));
            if (((Header.Size ^ Header.NotSize) == 0xFF)
                && (Header.DstNode == NodeId || Header.DstNode == BROADCAST_NODE_ID)) {
              State = STATE_RX_PAYLOAD;
            } else {
              reset();
            }
          }
          break;
        case STATE_RX_PAYLOAD:
          if (uart->rxnum() >= Header.Size) {
            uart->rxFifo.read((u08*) payload, Header.Size);
            u08 calcCrc;
            calcCrc = crc8((u08*) payload, Header.Size);
            if (calcCrc == Header.CRC) {
              State = STATE_PACKET_AVAILABLE;
            } else {
              reset();
            }
          }
          break;
        case STATE_PACKET_AVAILABLE:
          break;
        default:
          State = STATE_RX_HEADER;
          break;
      }
      if (timeout.isSet()) {
        reset();
      }
    }
  }
}
/****************************************************************************************/
void Cnetwork::tx(u08 transactNum, u08 dstNode, u08* Dat, u08 byteCnt) {

  sHeader Header;
  Header.Header = HEADER;
  Header.Size = byteCnt;
  Header.NotSize = (byteCnt ^ 0xFF);
  Header.DstNode = dstNode;
  Header.SrcNode = NodeId;
  Header.TransactNum = transactNum;
  Header.CRC = crc8(Dat, byteCnt);
  reset();
  uart->write((c08*) &Header, sizeof(Header));
  uart->write((c08*) Dat, byteCnt);
}
