/****************************************************************************************/
// The Cserial class is the interface definition for serial driver classes.
//
//   Created : 29 Feb 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
/****************************************************************************************/
#ifndef SERIAL_H_
#define SERIAL_H_
/****************************************************************************************/
#include "types.h"
#include "fifo.h"
/****************************************************************************************/
class Cserial {
  protected:
    u16 rxOverflowCnt;
    u08 txCnt;
    u16 lastTxTime;
    u16 baseAdr;
  public:
    u08 txBusy;
    u08 healthy;
    u32 baudRate;
    Tfifo<u08> rxFIFO;
    Tfifo<u08> txFIFO;
    Cserial(void);
    virtual void service(void);
    virtual u16 write(u08* buffer, u16 nBytes) = 0;
    virtual bool start(void) = 0;
    u16 read(u08* buffer, u16 nBytes);
    u16 read(u08* buffer);
    void clearRx(void);
    void clearTx(void) {
      txFIFO.clear();
    }
    u16 space(void);
    u16 rxnum(void);
    void uprintf(const char *__fmt, ...);
    void sendStr_P(const prog_char str[]);
    void sendStr(c08* str);
    u16 send_P(const prog_char buf[], u16 nBytes);
    void setBufSize(u16 bufSize);
    virtual void setBaudRate(u32 baudRate);
    u32 getBaudRate(void) {
      return baudRate;
    }
};
/****************************************************************************************/
#endif /* SERIAL_H_ */
