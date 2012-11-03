#include <string.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "transport.h"

#define TX_PERIOD 750000  //us
#define MAX_RE_TX 10

Ctransport::Ctransport(Cnetwork* _network) {
  u08 cnt;
  for (cnt = 0; cnt < MAX_TRANSACTIONS; cnt++) {
    txList[cnt].used = false;
    txList[cnt].done = false;
    txList[cnt].cntTx = 0;
  }
  txIndex = 0;
  packetAvailable = false;
  network = _network;
  isr_time = 0;
  time = 0;
  busy = false;
}

void Ctransport::service(void) {
  u08 cnt;
  if(rx()){
    return;
  }

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    time = isr_time;
  }
  if (time > TX_PERIOD || !busy) {
    time = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      isr_time = time;
    }
    cnt = 0;
    while (cnt < MAX_TRANSACTIONS) {
      if (txList[txIndex].used == true && txList[txIndex].done == false) {
        if (txList[txIndex].cntTx > MAX_RE_TX) {
          txList[txIndex].used = false;
          txList[txIndex].done = false;
          txList[txIndex].timeout = true;
          txList[txIndex].cntTx = 0;
        } else {
          network->tx(txIndex, txList[txIndex].nodeId, txList[txIndex].txDat, txList[txIndex].len);
          txList[txIndex].cntTx++;
          txIndex = (txIndex + 1) % MAX_TRANSACTIONS;
          busy = true;
        }
        break;
      }
      txIndex = (txIndex + 1) % MAX_TRANSACTIONS;
      cnt++;
    }
  }
}

bool Ctransport::rx(void) {
  u08 index;

  if (network->packetAvailable()) {
    index = network->Header.TransactNum;
    if (index < MAX_TRANSACTIONS) {
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
    case HANDLE_EMPTY:
      i = getEmptyIndex();
      if (i < MAX_TRANSACTIONS) {
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
        i = HANDLE_EMPTY;
      }
      ret = i;
      break;
      //========================================================================
      //  Reset the handle
    case HANDLE_DONE:
      ret = HANDLE_EMPTY;
      break;
      //========================================================================
      //  Respond with existing transaction in Queue
    default:
      if (_handle < MAX_TRANSACTIONS) {
        if (txList[_handle].timeout) {
          txList[_handle].timeout = false;
          txList[_handle].used = false;
          ret = HANDLE_EMPTY;
        } else if (txList[_handle].done) {
          txList[_handle].used = false;
          *ppRsp = (u08*) &txList[_handle].rxDat[0];
          ret = HANDLE_DONE;
        } else {
          ret = _handle;
        }
      } else {
        ret = HANDLE_EMPTY;
      }
      break;
  }
  return ret;
}

//========================================================================
u08 Ctransport::getEmptyIndex(void) {
  u08 cnt;
  for (cnt = 0; cnt < MAX_TRANSACTIONS; cnt++) {
    if (!txList[cnt].used) {
      return cnt;
    }
  }
  return HANDLE_EMPTY;
}

