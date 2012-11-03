#ifndef CNETWORK_H_
#define CNETWORK_H_
/****************************************************************************************/
#include "scheduler.h"
#include "types.h"
#include "uart.h"
/****************************************************************************************/
namespace CNETWORK {
  /****************************************************************************************/
#define NUM_PAYLOAD_BYTES   128
  /****************************************************************************************/
#define HEADER              0x5A
  /*********************************** Packet struct definition ***************************/
  typedef struct {
      u08 Header;
      u08 Size;
      u08 NotSize;
      u08 DstNode;
      u08 SrcNode;
      u08 TransactNum;
      u08 CRC;
  } sHeader;
  /****************************************************************************************/
  typedef enum {
    STATE_RX_HEADER,
    STATE_RX_SIZE,
    STATE_RX_NOT_SIZE,
    STATE_RX_DST_NODE,
    STATE_RX_SRC_NODE,
    STATE_RX_TRANSACT_NUM,
    STATE_RX_CRC,
    STATE_RX_PAYLOAD,
    STATE_SKIP_PAYLOAD,
    STATE_PACKET_AVAILABLE
  } eState;
  /****************************************************************************************/
  class Cnetwork : public Csignal{
    private:
      static const u08 BROADCAST_NODE_ID = 0;
      static const u08 UNCONF_NODE_ID = 0;
      static const f32 timeoutTime = 1;
      static const f32 period = 10e-3;
      Csignal timeout;
      Cuart* uart;
      u08 cntByte;
      u32 timeLimit;
      u16 baudRate;
      u08 payloadSize;
    public:
      eState State;
      u08 NodeId;
      u08 healthy;
      volatile u32 time;
      sHeader Header;
      u08* payload;
      Cnetwork(Cuart* _uartNr, u08 _node, u08 _size = 128);
      u08 setPayloadBufSize(u08 size);
      void service(void);
      u08 packetAvailable(void);
      void reset(void);
      void tx(u08 transactNum, u08 dstNode, u08* dat, u08 byteCnt);
  };
/****************************************************************************************/
}
#endif
