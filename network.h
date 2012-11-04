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
      static const f32 timeoutTime = 4;
      static const f32 period = 100e-3;
      Csignal timeout;
      Cuart* uart;
      u16 baudRate;
      u08 payloadSize;
    public:
      Cnetwork(Cuart* _uartNr, u08 _node, u08 _size = 0x60);
      void setPayloadBufSize(u08 size);
      void service(void);
      void tx(u08 transactNum, u08 dstNode, u08* dat, u08 byteCnt);
      void reset(void) {
        uart->clearRx();
        State = STATE_RX_HEADER;
      }
      u08 packetAvailable(void) {
        return (State == STATE_PACKET_AVAILABLE);
      }

      eState State;
      u08 NodeId;
      sHeader Header;
      u08* payload;
  };
/****************************************************************************************/
}
#endif
