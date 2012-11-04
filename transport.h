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

typedef enum {
  handleEmpty = 0xFE, handleDone = 0xFD
} eHandle;

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

class Ctransport: public Csignal {
  private:
    static const f32 period = 750e-3;
    static const u08 maxReTx = 10;
    static const u08 maxTransactions = 16;
    static const u08 maxWait = 4;
    Cnetwork* network;
    sTxList txList[maxTransactions];
    u08 txIndex;
    u08 getEmptyIndex(void);
    u32 time;
    volatile u08 busy;
  public:
    Ctransport(Cnetwork* _network);
    bool read(u08* rxDat, u08* txDat, u08 byteCnt, u08 dstNode);
    void service(void);
    u08 tx(u08 _handle, u08 nodeId, u08* pDat, u08 numBytes, u08** ppRsp);
    bool rx(void);

    sHeader rxHeader;
    u08 transactNum[maxTransactions];
    u08 txNum;
    u08 packetAvailable;
};

#endif /* CTRANSPORT_H_ */
