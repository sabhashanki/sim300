#ifndef CUART_H
#define CUART_H

#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "common.h"
#include "fifo.h"

//#define UART_MINIMAL

#ifdef SIG_UART_DATA
#warning UART REGISTERS FOR MEGA 8
extern "C" void SIG_UART_DATA(void) __attribute__ ((signal));
extern "C" void SIG_UART_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_UART_RECV(void) __attribute__ ((signal));
#endif

#ifdef SIG_USART0_DATA
extern "C" void SIG_USART0_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART0_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART0_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR1
extern "C" void SIG_USART1_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART1_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART1_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR2
extern "C" void SIG_USART2_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART2_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART2_RECV(void) __attribute__ ((signal));
#endif

#ifdef UDR3
extern "C" void SIG_USART3_DATA(void) __attribute__ ((signal));
extern "C" void SIG_USART3_TRANS(void) __attribute__ ((signal));
extern "C" void SIG_USART3_RECV(void) __attribute__ ((signal));
#endif

class Cuart {
  private:
    u16 rxOverflowCnt;
    u08 uartNr;
    volatile u08* port485;
    u08 pin485;
    u08 txCnt;
    u16 lastTxTime;
  public:
    Tfifo<u08> rxFifo;
    Tfifo<u08> txFifo;
    //volatile u32 time;
    u08 txBusy;
    u08 enable485;
    u08 healthy;
    u32 baudRate;
    Cuart(u08 uartNr, u32 baudRate = 9600, u16 bufSize = 64, bool enable485 = false);
    u16 write(c08* buffer, u16 nBytes);
    u16 write(Tfifo<u08>* dat);
#ifndef UART_MINIMAL
    u16 send_P(const prog_char buf[], u16 nBytes);
    void sendStr_P(const prog_char str[]);
    void sendStr(c08* str);
    void uprintf(const char *__fmt, ...);
#endif
    u16 read(u08* buffer, u16 nBytes = 0, bool keep = false);
    u16 space(void);
    u16 rxnum(void);
    void clearRx(void);
    void clear(void) {
      txFifo.clear();
      rxFifo.clear();
    }
    void setBaudRate(u32 baudrate);
    void setFrame(void);
    /// Interrupt routines.
#ifdef UDR
    friend void SIG_UART_DATA(void);
    friend void SIG_UART_TRANS(void);
    friend void SIG_UART_RECV(void);
#endif
#ifdef UDR0
    friend void SIG_USART0_DATA(void);
    friend void SIG_USART0_TRANS(void);
    friend void SIG_USART0_RECV(void);
#endif
#ifdef UDR1
    friend void SIG_USART1_DATA(void);
    friend void SIG_USART1_TRANS(void);
    friend void SIG_USART1_RECV(void);
#endif
#ifdef UDR2
    friend void SIG_USART2_DATA(void);
    friend void SIG_USART2_TRANS(void);
    friend void SIG_USART2_RECV(void);
#endif
#ifdef UDR3
    friend void SIG_USART3_DATA(void);
    friend void SIG_USART3_TRANS(void);
    friend void SIG_USART3_RECV(void);
#endif

};

#endif

