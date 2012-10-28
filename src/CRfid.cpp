/*
 * CRfid.cpp
 *
 *  Created on: 13 Aug 2009
 *      Author: Pieter
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>

#include "common.h"
#include "CServer.h"
#include "CRfid.h"


#define DEBUG_RFID 2

#if (DEBUG_RFID)
extern CUART DbgUart;
#endif

//using namespace RFID;

CRFID::CRFID(CUART *_pUart, u08 _tableLen, CServer *_server) {
  pUart = _pUart;
  server = _server;
  tableLen = _tableLen;
  pTable = (sRfidTag*) malloc((sizeof(sRfidTag) * _tableLen));
  memset((u08*) pTable, 0, (sizeof(sRfidTag) * _tableLen));
  cntByte = 0;
  maxPersistCnt = 30;//in seconds
  prevtimer = 0;
  timer = 0;
  newTag.tagID = 0;
  signal[RFID_TABLE_SIGNAL].period = T_1SEC;
  signal[RFID_TABLE_SIGNAL].timer = 0;

  signal[RFID_TAG_SIGNAL].period = T_100MS;
  signal[RFID_TAG_SIGNAL].timer = 0;

  signal[RFID_NEWTAG_TIMEOUT_SIGNAL].period = 30 * T_1SEC;
  signal[RFID_NEWTAG_TIMEOUT_SIGNAL].timer = 0;
}

void CRFID::clearTable(void) {
  memset((u08*) pTable, 0, (sizeof(sRfidTag) * tableLen));
}

/****************************************************************************
 Update the tags persistence counters
 ****************************************************************************/
void CRFID::Service(void) {
  u08 i;
  sRfidTag rfidTag;

  ServiceSignals();
  // read data off the uart and if a valid tag is received
  // compare it to what is already in the table
  if (ReceiveTag(&rfidTag)) {
    /* Update persist count - or add a new tag */
    UpdateTag(&rfidTag);
  }
  if (signal[RFID_TABLE_SIGNAL].state == SIG_TRIGGERED) {
    signal[RFID_TABLE_SIGNAL].state = SIG_RESET;
#if (DEBUG_RFID >=3)
    DbgUart.uprintf("\n\rTAG TABLE : ");
#endif
    for (i = 0; i < tableLen; i++) {
      // decrement persist counter - tag not seen but was in table
      if (pTable[i].persistCnt > 1) {
        pTable[i].persistCnt--;
#if (DEBUG_RFID >= 3)
        DbgUart.uprintf("\t[T:=%d-%d c:%d S:=%d]",(u16)pTable[i].custCode,(u16)pTable[i].tagID,(u16)pTable[i].persistCnt,(u16)pTable[i].rssi);
#endif
      } else if ((pTable[i].persistCnt == 1 || pTable[i].rssi < 100) && pTable[i].status == TAG_PRESENT) {
        pTable[i].status = TAG_LOST;
        pTable[i].srvlogged = false;
        pTable[i].smslogged = false;
        //pTable[i].persistCnt = 0;
#if (DEBUG_RFID >= 2)
        DbgUart.uprintf("\n\rTAG_LOST");
        DbgUart.uprintf("\t[T:=%d C:%d S:=%d]", (u16) pTable[i].tagID,
                        (u16) pTable[i].persistCnt, (u16) pTable[i].rssi);
#endif
      }
    }
  }
}

/****************************************************************************
 Search in the table for the RfidTag that was just received from the pUart
 ****************************************************************************/
u08 CRFID::SearchTagID(u32 tagID, u08 *tableIndex) {
  u08 i;
  for (i = 0; i < tableLen; i++) {
    if ((pTable[i].tagID == tagID)) {// &&( pRfidTag->custCode == pTable[i].custCode))  {
      *tableIndex = i;
      return true;
    }
  }
  *tableIndex = 0;
  return false;
}
/****************************************************************************

 ****************************************************************************/
u08 CRFID::AddNewTag(sRfidTag *pRfidTag) {
  u08 i;
  if (newTag.tagID == 0) {
    newTag.tagID = pRfidTag->tagID;
    DbgUart.uprintf("\n\rPickup a Tag:\t%d ", (u16) newTag.tagID);
    newTag.persistCnt = 0;
    signal[RFID_NEWTAG_TIMEOUT_SIGNAL].state = SIG_RESET;
  } else if (newTag.tagID == pRfidTag->tagID) {
    newTag.persistCnt++;
    //store the tag
    if (newTag.persistCnt > 2) {
      DbgUart.uprintf("\n\rAdd New TAG:\t%d cnt=%d", (u16) newTag.tagID,
                      (u16) newTag.persistCnt);
      for (i = 0; i < tableLen; i++) {
        //Get Empty slot
        if (pTable[i].persistCnt == 0) {
          pTable[i] = *pRfidTag;
          pTable[i].status = TAG_PRESENT;
          pTable[i].persistCnt = maxPersistCnt;
          pTable[i].srvlogged = false;
          pTable[i].smslogged = false;
          newTag.tagID = 0;
          return true;
        }
      }
    }
  } else {
    if (signal[RFID_NEWTAG_TIMEOUT_SIGNAL].state == SIG_TRIGGERED) {
      newTag.tagID = 0;
    }

  }
  return false;
}
/****************************************************************************
 Update the persistance counter of pRfidTag in the table
 ****************************************************************************/
u08 CRFID::UpdateTag(sRfidTag *pRfidTag) {
  u08 tableIndex;

  if (SearchTagID(pRfidTag->tagID, &tableIndex)) {
    pTable[tableIndex].persistCnt = maxPersistCnt;
    pTable[tableIndex].rssi = pRfidTag->rssi;
    pTable[tableIndex].duress = pRfidTag->duress;
    return true;
  } else {
    if (AddNewTag(pRfidTag)) {
      server->SetTagInRange(pRfidTag->tagID);
    }

  }
  return false;
}

void CRFID::ServiceSignals(void) {
  u08 i;
  for (i = 0; i < NUM_RFID_SIGNALS; i++) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      if (timer) {
        if (signal[i].timer < 0xFFFFFFFF && signal[i].state != SIG_TRIGGERED) {
          signal[i].timer += timer;
          timer = 0;
        }
      }
    }
    if (signal[i].state == SIG_RESET) {
      signal[i].timer = 0;
      signal[i].state = SIG_PENDING;
    } else if (signal[i].timer > signal[i].period) {
      signal[i].state = SIG_TRIGGERED;
    }
  }
}

u08 CRFID::ReceiveTag(sRfidTag *pRfidTag) {
  u08 crc;
  u08 ret;

  ret = false;
  if (signal[RFID_TAG_SIGNAL].state == SIG_TRIGGERED) {
    rxState = STATE_RFID_HEADER1;
    cntByte = 0;
  }

  switch (rxState) {
    case STATE_RFID_HEADER1:
      if (pUart->receive(&payload[0], 1) == 1) {
        if (payload[cntByte] == RFID_HEADER1) {
          rxState = STATE_RFID_HEADER2;
          signal[RFID_TAG_SIGNAL].state = SIG_RESET;
        }
      }
      break;
    case STATE_RFID_HEADER2:
      if (pUart->receive(&payload[cntByte], 1) == 1) {
        if (payload[cntByte] == RFID_HEADER2) {
          rxState = STATE_RFID_HEADER3;
          signal[RFID_TAG_SIGNAL].state = SIG_RESET;
        }
      }
      break;
    case STATE_RFID_HEADER3:
      if (pUart->receive(&payload[cntByte], 1) == 1) {
        if (payload[cntByte] == RFID_HEADER3) {
          cntByte = 0;
          rxState = STATE_RFID_PAYLOAD;
          signal[RFID_TAG_SIGNAL].state = SIG_RESET;
        }
      }
      break;
    case STATE_RFID_PAYLOAD:
      if (pUart->receive(&payload[cntByte], 1) == 1) {
        cntByte++;
        signal[RFID_TAG_SIGNAL].state = SIG_RESET;
        if (cntByte == RFID_PACKET_LEN) {
          crc = 0;
          for (cntByte = 0; cntByte < 18; cntByte++) {
            crc += payload[cntByte];
          }
          if (crc == payload[18] && payload[19] == RFID_ETX1 && payload[20] == RFID_ETX2) {
            /* CUST CODE */
            pRfidTag->custCode = payload[2] + payload[3];
            /* TAG NUMBER */
            pRfidTag->tagID = payload[4];
            pRfidTag->tagID = (pRfidTag->tagID << 8) + payload[5];
            pRfidTag->tagID = (pRfidTag->tagID << 8) + payload[6];
            /* SIGNAL STENGTH*/
            pRfidTag->rssi = u16((((u16) payload[9]) << 8) + (u16) payload[10]);
            /* DURESS FLAGS */
            pRfidTag->duress = payload[11];
            if (pRfidTag->tagID == 0)
              ret = false;
            else
              ret = true;
          } else {
            ret = false;
          }
          rxState = STATE_RFID_HEADER1;
          cntByte = 0;
        }
      }
      break;
    default:
      rxState = STATE_RFID_HEADER1;
      cntByte = 0;
      break;
  }
  return ret;
}
#if 0
========== SNAPSHOT of DATA ===============
HEADER [ 0.. 4] : 00 00 21 2A 2A
HEADER [ 5.. 6] : 00 37
CUSTCO [ 7.. 8] : 00 1D
ID [ 9..11] : 00 00 11
?? [12 ] : 4C
COUNTER[13 ] : B7 // 0x00 - 0xFF
RSSI [14..15] : 02 58
DURESS [16 ] : 00 //0x42 when pressed
?? [17 ] : 00
COUNTER[18 ] : 2F // 0x00 - 0xFF
?? [19..22] : 00 00 00 00
CRC [23..24] : F1
ETX [25..26] : 0A 0D

HEADER 00 00 21 2A 2A

/* ANDER TAG*/
00 00 21 2A 2A 00 37 00 16 00 00 11 29 C3 02 75 00 00 4E 00 00 00 00 0F 0A 0D
00 00 21 2A 2A 00 37 00 16 00 00 11 29 C4 02 75 00 00 4F 00 00 00 00 11 0A 0D
00 00 21 2A 2A 00 37 00 16 00 00 11 29 C5 02 74 00 00 50 00 00 00 00 12 0A 0D

00 37 00 1D 00 00 11 4C B8 02 58 00 00 30 00 00 00 00 F3 0A 0D
/* DURRES*/

00 00 21 2A 2A 00 37 00 1D 00 00 11 4C BB 02 56 42 00 33 00 00 00 00 39 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C BC 02 56 42 00 34 00 00 00 00 3B 0A 0D
00 00 21 2A 2A 00 37 80 1D 00 00 11 4C BD 02 57 42 00 35 00 00 00 00 BE 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C BE 02 57 42 00 36 00 00 00 00 40 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C BF 02 58 42 00 37 00 00 00 00 43 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C0 02 57 42 00 38 00 00 00 00 44 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C1 02 56 42 00 39 00 00 00 00 45 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C2 02 56 42 00 3A 00 00 00 00 47 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C3 02 57 42 00 3B 00 00 00 00 4A 0A 0D
/* NORMAL */
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C4 02 58 00 00 3C 00 00 00 00 0B 0A 0D
00 00 21 2A 2A 00 37 80 1D 00 00 11 4C C5 02 58 00 00 3D 00 00 00 00 8D 0A 0D
/* DURRES*/
00 00 21 2A 2A 00 37 00 1D 80 80 11 4C C6 02 55 42 00 3E 00 00 00 00 4E 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C7 02 56 42 00 3F 00 00 00 00 51 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C8 02 54 42 00 40 00 00 00 00 51 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C C9 02 56 42 00 41 00 00 00 00 55 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CA 02 56 42 00 42 00 00 00 00 57 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CB 02 58 42 00 43 00 00 00 00 5B 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CC 02 58 42 00 44 00 00 00 00 5D 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CD 02 58 42 00 45 00 00 00 00 5F 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CE 02 57 42 00 46 00 00 00 00 60 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C CF 02 58 42 00 47 00 00 00 00 63 0A 0D
/* NO DURRES*/
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D0 02 58 00 00 48 00 00 00 00 23 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D1 02 59 00 00 49 00 00 00 00 26 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D2 02 58 00 00 4A 00 00 00 00 27 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D3 02 59 00 00 4B 00 00 00 00 2A 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D4 02 59 00 00 4C 00 00 00 00 2C 0A 0D
00 00 21 2A 2A 00 37 80 1D 00 00 11 4C D5 02 58 00 00 4D 00 00 00 00 AD 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D6 02 59 00 00 4E 00 00 00 00 30 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D7 02 58 00 00 4F 00 00 00 00 31 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D8 02 59 00 00 50 00 00 00 00 34 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C D9 02 59 00 00 51 00 00 00 00 36 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C DA 02 59 00 00 52 00 00 00 00 38 0A 0D
00 00 21 2A 2A 00 37 00 1D 00 00 11 4C DB 02 59 00 00 53 00 00 00 00 3A 0A 0D

#endif
