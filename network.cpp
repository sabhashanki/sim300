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
  time = 0;
  timeLimit = 0;
  payloadSize = 0;
  payload = 0;
  healthy = true;
  baudRate = this->uart->baudRate;
  setPayloadBufSize(_size);
  scheduler.attach(&timeout);
  scheduler.attach(this);
}
/****************************************************************************************/
u08 Cnetwork::setPayloadBufSize(u08 _size) {
  payload = (u08*) malloc(_size);
  if (payload == NULL) {
    healthy = false;
    return false;
  }
  payloadSize = _size;
  return true;
}
/****************************************************************************************/
void Cnetwork::service(void) {
  if (isSet()) {
    while (uart->rxnum()) {
      switch (State) {
        case STATE_RX_HEADER:
          if (uart->rxFifo.read(&Header.Header, 1) == 1) {
            if (Header.Header == HEADER) {
              cntByte = 0;
              State = STATE_RX_SIZE;
            }
          }
          break;
        case STATE_RX_SIZE:
          if (uart->rxFifo.read(&Header.Size, 1) == 1) {
            State = STATE_RX_NOT_SIZE;
          }
          break;
        case STATE_RX_NOT_SIZE:
          if (uart->rxFifo.read(&Header.NotSize, 1) == 1) {
            if ((Header.Size ^ Header.NotSize) == 0xFF) {
              State = STATE_RX_DST_NODE;
            } else {
              State = STATE_RX_HEADER;
            }
          }
          break;
        case STATE_RX_DST_NODE:
          if (uart->rxFifo.read(&Header.DstNode, 1) == 1) {
            State = STATE_RX_SRC_NODE;
          }
          break;
        case STATE_RX_SRC_NODE:
          if (uart->rxFifo.read(&Header.SrcNode, 1) == 1) {
            State = STATE_RX_TRANSACT_NUM;
          }
          break;
        case STATE_RX_TRANSACT_NUM:
          if (uart->rxFifo.read(&Header.TransactNum, 1) == 1) {
            State = STATE_RX_CRC;
          }
          break;
        case STATE_RX_CRC:
          if (uart->rxFifo.read(&Header.CRC, 1) == 1) {
            cntByte = 0;
            if (Header.DstNode == NodeId || Header.DstNode == BROADCAST_NODE_ID) {
              State = STATE_RX_PAYLOAD;
            } else {
              State = STATE_SKIP_PAYLOAD;
            }
          }
          break;
        case STATE_SKIP_PAYLOAD:
          if (uart->rxFifo.read(&payload[0], 1) == 1) {
            cntByte++;
            if (cntByte == Header.Size) {
              State = STATE_RX_HEADER;
            }
          }
          break;
        case STATE_RX_PAYLOAD:
          if (uart->rxFifo.read(&payload[cntByte], 1) == 1) {
            cntByte++;
            if (cntByte == Header.Size) {
              u08 calcCrc;
              calcCrc = crc8((u08*) payload, Header.Size);
              if ((calcCrc == Header.CRC)
                  && (Header.DstNode == NodeId || Header.DstNode == BROADCAST_NODE_ID)) {
                State = STATE_PACKET_AVAILABLE;
              } else {
                State = STATE_RX_HEADER;
              }
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
        State = STATE_RX_HEADER;
      }
    }
  }
}
/****************************************************************************************/
void Cnetwork::reset(void) {
  State = STATE_RX_HEADER;
}
/****************************************************************************************/
u08 Cnetwork::packetAvailable(void) {
  if (State == STATE_PACKET_AVAILABLE) {
    return true;
  }
  return false;
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
  State = STATE_RX_HEADER;
  uart->write((c08*) &Header, sizeof(Header));
  uart->write((c08*) Dat, byteCnt);
}
