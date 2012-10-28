/*
 * CRfid.cpp
 *
 *  Created on: 13 Aug 2009
 *      Author: Pieter
 */

//=============================================================================
#ifndef CRFID_H_
#define CRFID_H_

#include "common.h"
#include "avrlibtypes.h"
#include "avrlibdefs.h"
#include "main.h"
#include "CServer.h"
#include "CModem.h"
#include "CUART.h"

#define RFID_HEADER1 0x21
#define RFID_HEADER2 0x2A
#define RFID_HEADER3 0x2A
#define RFID_ETX1    0x0A
#define RFID_ETX2    0x0D

#define NUM_RFID_SIGNALS  3
#define RFID_TABLE_SIGNAL 0
#define RFID_TAG_SIGNAL   1
#define RFID_NEWTAG_TIMEOUT_SIGNAL   2
#define RFID_PACKET_LEN 21

#define T_100MS   100000
#define T_1SEC   1000000
#define T_60SEC  6000000
#define T_10MIN 60000000


//======================================================================
typedef struct
{
  u16 persistCnt;
  u08 status;
  u32 tagID;
  u16 custCode;
  u16 rssi;
  u08 duress;
  u08 srvlogged;
  u08 smslogged;
} sRfidTag;

typedef enum
{
  SIG_TRIGGERED = 2, SIG_RESET, SIG_PENDING, SIG_STOP
} eSignalState;

typedef struct
{
  u32 period;
  volatile u32 timer;
  u08 state;
} sSignal;

typedef enum
{
  STATE_RFID_HEADER1,
  STATE_RFID_HEADER2,
  STATE_RFID_HEADER3,
  STATE_RFID_HEADER4,
  STATE_RFID_SIZE,
  STATE_RFID_CRC,
  STATE_RFID_PAYLOAD,
  STATE_PACKET_AVAILABLE
} eRxState;

//======================================================================
typedef enum
{
  TAG_NONE = 0, TAG_PRESENT = 2, TAG_LOST = 4, TAG_DURESS = 8
} eRfidTagState;

  class CRFID
  {
  private:
    CUART *pUart;
    CServer *server;
    eRxState rxState;
    u08 cntByte;
    u08 payload[RFID_PACKET_LEN];
    sRfidTag newTag;
    u08 crc8(u08 crc, u08 data);
    u16 maxPersistCnt;
    sSignal signal[NUM_RFID_SIGNALS];
    u32 prevtimer;
    u08 AddNewTag(sRfidTag *pRfidTag);
    u08 UpdateTag(sRfidTag *pRfidTag);
    u08 ReceiveTag(sRfidTag *pRfidTag);
    void ServiceSignals(void);
  public:
    u08 tableLen;
    sRfidTag *pTable;
    u32 timer;
    CRFID(CUART *_pUart, u08 _tableLen,CServer *_server);
    void Service(void);
    void clearTable(void);
    u08 SearchTagID(u32 tagID, u08 *tableIndex);
  };
//}
#endif
