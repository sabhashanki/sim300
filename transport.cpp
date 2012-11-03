#include <string.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "transport.h"

Ctransport::Ctransport(Cnetwork* _network) :
    Csignal(period) {
  u08 cnt;
  for (cnt = 0; cnt < maxTransactions; cnt++) {
    txList[cnt].used = false;
    txList[cnt].done = false;
    txList[cnt].cntTx = 0;
  }
  txIndex = 0;
  packetAvailable = false;
  network = _network;
  time = 0;
  busy = false;
  scheduler.attach(this);
  txNum = 0;
}

bool Ctransport::read(u08* rxDat, u08* txDat, u08 byteCnt, u08 dstNode) {
  start(300e-3);
  network->tx(txNum, dstNode, txDat, byteCnt);
  while (!network->packetAvailable()) {
    network->service();
    if (isSet()) {
      return false;
    }
  }
  memcpy(rxDat, network->payload, network->Header.Size);
  network->reset();
  return true;
}

void Ctransport::service(void) {
  u08 cnt = 0;

  rx();
  if (isSet()) {
    while (cnt < maxTransactions) {
      if (txList[txIndex].used && !txList[txIndex].done) {
        if (txList[txIndex].cntTx > maxReTx) {
          txList[txIndex].used = false;
          txList[txIndex].done = false;
          txList[txIndex].timeout = true;
          txList[txIndex].cntTx = 0;
        } else {
          network->tx(txIndex, txList[txIndex].nodeId, txList[txIndex].txDat,
                      txList[txIndex].len);
          txList[txIndex].cntTx++;
          txIndex = (txIndex + 1) % maxTransactions;
          busy = true;
          return;
        }
      }
      txIndex = (txIndex + 1) % maxTransactions;
      cnt++;
    }
  }
}

bool Ctransport::rx(void) {
  u08 index;

  if (network->packetAvailable()) {
    index = network->Header.TransactNum;
    if (index < maxTransactions) {
      memcpy(txList[index].rxDat, network->payload, network->Header.Size);
      txList[index].done = true;
      txList[index].cntTx = 0;
      txList[index].used = false;
    }
    network->reset();
    busy = false;
    return true;
  }
  return false;
}

u08 Ctransport::tx(u08 _handle, u08 nodeId, u08* pDat, u08 numBytes, u08** ppRsp) {
  u08 i;
  u08 ret;
  switch (_handle) {
    //========================================================================
    //  Create a new transaction - get first available handle/index
    case handleEmpty:
      i = getEmptyIndex();
      if (i < maxTransactions) {
        txList[i].used = true;
        txList[i].nodeId = nodeId;
        memcpy(txList[i].txDat, pDat, numBytes);
        memset(txList[i].rxDat, 0, sizeof(txList[i].rxDat));
        txList[i].len = numBytes;
        //txList[index].rsp = Rsp;
        txList[i].done = false;
        txList[i].cntTx = 0;
        txList[i].timeout = false;
      } else {
        i = handleEmpty;
      }
      ret = i;
      break;
      //========================================================================
      //  Reset the handle
    case handleDone:
      ret = handleEmpty;
      break;
      //========================================================================
      //  Respond with existing transaction in Queue
    default:
      if (_handle < maxTransactions) {
        if (txList[_handle].timeout) {
          txList[_handle].timeout = false;
          txList[_handle].used = false;
          ret = handleEmpty;
        } else if (txList[_handle].done) {
          txList[_handle].used = false;
          *ppRsp = (u08*) &txList[_handle].rxDat[0];
          ret = handleDone;
        } else {
          ret = _handle;
        }
      } else {
        ret = handleEmpty;
      }
      break;
  }
  return ret;
}

//========================================================================
u08 Ctransport::getEmptyIndex(void) {
  u08 cnt;
  for (cnt = 0; cnt < maxTransactions; cnt++) {
    if (!txList[cnt].used) {
      return cnt;
    }
  }
  return handleEmpty;
}

