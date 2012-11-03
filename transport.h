/*
 * CTransport.h
 *
 *  Created on: 01 Mar 2009
 *      Author: Wouter
 */

#ifndef CTRANSPORT_H_
#define CTRANSPORT_H_

#include <stdlib.h>
#include "types.h"
#include "network.h"

#define HANDLE_EMPTY  0xFE
#define HANDLE_DONE   0xFD

#define MAX_TRANSACTIONS 16

using namespace CNETWORK;

typedef struct {
  u08 used;
  u08 done;
  u08 timeout;
  u08 nodeId;
  u08 rxDat[NUM_PAYLOAD_BYTES];/* GROOT HACK*/
  u08 txDat[NUM_PAYLOAD_BYTES];/* GROOT HACK*/
  u08 len;
  u08 *rsp;
  u16 cntTx;
} sTxList;


class Ctransport {
private:
  Cnetwork* network;
  sTxList txList[MAX_TRANSACTIONS];
  u08 txIndex;
  u08 getEmptyIndex(void);
  u32 time;
  volatile u08 busy;
public:
  volatile u32 isr_time;
  sHeader rxHeader;
  u08 transactNum[MAX_TRANSACTIONS];
  u08 packetAvailable;
  Ctransport(Cnetwork* _network);
  void service(void);
  u08 tx(u08 _handle, u08 nodeId, u08* pDat, u08 numBytes,u08** ppRsp);
  bool rx(void);
};

#endif /* CTRANSPORT_H_ */
