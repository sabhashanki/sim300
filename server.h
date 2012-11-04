#ifndef SERVER_H_
#define SERVER_H_
/****************************************************************************************/
#include "types.h"
#include "serial.h"
#include "scheduler.h"
/****************************************************************************************/
namespace NS_CSERVER {
  typedef enum {
    T_BOOL = 0, T_CHAR = 1, T_UCHAR = 2, T_STRING = 11
  } eDataTyep;
  /****************************************************************************************/
  typedef enum {
    U2S_STATUSUPDATE = 1, U2S_OPEN_REQUEST,
  } eMsgID;
  /****************************************************************************************/
  typedef enum {
    E_UNIT_ID = 0, E_UNIT_GPS = 1, E_COVER_RFID = 2, E_COVER_STATUS = 3
  } eMsgType;
  /****************************************************************************************/
  typedef enum {
    COVER_CLOSED = 0, COVER_OPEN = 1,
  } eCoverStatus;
  /****************************************************************************************/
  typedef struct {
      u32 Length;
      u08 MsgID;
  } sHeader;
  /****************************************************************************************/
  typedef struct {
      u08 elementID;
      u08 type;
      u08 strLen;
  } sDataStringHeader;
  /****************************************************************************************/
  typedef struct {
      u08 elementID;
      u08 type;
  } sDataBinHeader;
  /****************************************************************************************/
  typedef enum {
    STATE_RX_LENGTH, STATE_RX_MSG_ID, STATE_RX_PAYLOAD, STATE_PACKET_AVAILABLE
  } eState;
  /****************************************************************************************/
  class Cserver: public Csignal {
    private:
      static const u16 MAX_PACKET_LEN = 64;
      static const u16 period = 300e-3;
      Cserial* serial;
      u08 cntByte;
      u32 timeLimit;
      u16 baudRate;
      u08 payloadSize;
      Csignal timeout;
    public:
      eState State;
      u08 NodeId;
      u08 healthy;
      volatile u32 time;
      sHeader Header;
      u08* payload;
      Cserver(Cserial* serial, u08 size = MAX_PACKET_LEN, u08 node = 0);
      void sendCoverStatus(eCoverStatus status);
      bool statusUpdate(u08* strID, u08* strGPS, u08* strRFID, eCoverStatus* status);
      void setPayloadBufSize(u08 size);
      void service(void);
      u08 packetAvailable(void);
      void reset(void);
      void tx(u08 transactNum, u08 dstNode, u08* dat, u08 byteCnt);
      u08 nodeidGet(void);
      void nodeidSet(u08 ID);
      u16 htons(u16* val) {
        //u16 tmp;
        *val = ((*val << 8) | (*val >> 8));
        return *val;
        //*val = tmp;
      }
      u32 htonl(u32* val) {
        u16 tmpL, tmpR;
        tmpL = *val >> 16;
        tmpR = *val & 0x0000FFFF;
        *val = (htons(&tmpL) | (u32) htons(&tmpR) << 16);
        return *val;
      }
      void sendTestPkt();
      /****************************************************************************************/
  };
}
using namespace NS_CSERVER;

#endif
