#ifndef _CC1101_H
#define _CC1101_H

#include "types.h"
#include "fifo.h"
#include "spi.h"

/*****************************************************************************
 * TIME CONSTANTS
 *****************************************************************************/
#define CC1100_POWER_UP_DELAY_US                40
#define CC1100_MANCAL_DELAY_US                  721
#define CC1100_FS_WAKEUP_DELAY_US               45
#define CC1100_SETTLING_DELAY_US                44
#define CC1100_CALIBRATE_DELAY_US               720
#define CC1100_IDLE_NOCAL_DELAY_US              1
#define CC1100_TX_RX_DELAY_US                   22
#define CC1100_RX_TX_DELAY_US                   10

/*****************************************************************************/
typedef enum {
  eCC1100_TX_TYPE_FORCED = 1, eCC1100_TX_TYPE_CCA
} etxType;

/*****************************************************************************
 * Class: CC1101
 *****************************************************************************/
class CC1101 {
  private:
    /*****************************************************************************/
    static const u08 BURST_MASK      =  0x40;
    /*****************************************************************************/
    static const u08 RSSI_OFFSET     = 74;
    /*****************************************************************************/
    static const u08 CC1101_PATABLE  =         0x3E;
    static const u08 CC1101_TXFIFO   =         0x3F;
    static const u08 CC1101_RXFIFO   =         0x3F;
    /*****************************************************************************/
    // Command strobes for Registers from ADDR 0x30 to 0x3D
    // BURST BIT IS '0'
    /*****************************************************************************/
    static const u08 CC1101_SRES     =         0x30;
    static const u08 CC1101_SFSTXON  =         0x31;
    static const u08 CC1101_SXOFF    =         0x32;
    static const u08 CC1101_SCAL     =         0x33;
    static const u08 CC1101_SRX      =         0x34;
    static const u08 CC1101_STX      =         0x35;
    static const u08 CC1101_SIDLE    =         0x36;
    static const u08 CC1101_SWOR     =         0x38;
    static const u08 CC1101_SPWD     =         0x39;
    static const u08 CC1101_SFRX     =         0x3A;
    static const u08 CC1101_SFTX     =         0x3B;
    static const u08 CC1101_SWORRST  =         0x3C;
    static const u08 CC1101_SNOP     =         0x3D;
    /*****************************************************************************/
    // configuration registers
    /*****************************************************************************/
    static const u08 CC1101_IOCFG2   =         0x00;
    static const u08 CC1101_IOCFG1   =         0x01;
    static const u08 CC1101_IOCFG0   =         0x02;
    static const u08 CC1101_FIFOTHR  =         0x03;
    static const u08 CC1101_SYNC1    =         0x04;
    static const u08 CC1101_SYNC0    =         0x05;
    static const u08 CC1101_PKTLEN   =         0x06;
    static const u08 CC1101_PKTCTRL1 =         0x07;
    static const u08 CC1101_PKTCTRL0 =         0x08;
    static const u08 CC1101_ADDR     =         0x09;
    static const u08 CC1101_CHANNR   =         0x0A;
    static const u08 CC1101_FSCTRL1  =         0x0B;
    static const u08 CC1101_FSCTRL0  =         0x0C;
    static const u08 CC1101_FREQ2    =         0x0D;
    static const u08 CC1101_FREQ1    =         0x0E;
    static const u08 CC1101_FREQ0    =         0x0F;
    static const u08 CC1101_MDMCFG4  =         0x10;
    static const u08 CC1101_MDMCFG3  =         0x11;
    static const u08 CC1101_MDMCFG2  =         0x12;
    static const u08 CC1101_MDMCFG1  =         0x13;
    static const u08 CC1101_MDMCFG0  =         0x14;
    static const u08 CC1101_DEVIATN  =         0x15;
    static const u08 CC1101_MCSM2    =         0x16;
    static const u08 CC1101_MCSM1    =         0x17;
    static const u08 CC1101_MCSM0    =         0x18;
    static const u08 CC1101_FOCCFG   =         0x19;
    static const u08 CC1101_BSCFG    =         0x1A;
    static const u08 CC1101_AGCCTRL2 =         0x1B;
    static const u08 CC1101_AGCCTRL1 =         0x1C;
    static const u08 CC1101_AGCCTRL0 =         0x1D;
    static const u08 CC1101_WOREVT1  =         0x1E;
    static const u08 CC1101_WOREVT0  =         0x1F;
    static const u08 CC1101_WORCTRL  =         0x20;
    static const u08 CC1101_FREND1   =         0x21;
    static const u08 CC1101_FREND0   =         0x22;
    static const u08 CC1101_FSCAL3   =         0x23;
    static const u08 CC1101_FSCAL2   =         0x24;
    static const u08 CC1101_FSCAL1   =         0x25;
    static const u08 CC1101_FSCAL0   =         0x26;
    static const u08 CC1101_RCCTRL1  =         0x27;
    static const u08 CC1101_RCCTRL0  =         0x28;
    static const u08 CC1101_FSTEST   =         0x29;
    static const u08 CC1101_PTEST    =         0x2A;
    static const u08 CC1101_AGCTEST  =         0x2B;
    static const u08 CC1101_TEST2    =         0x2C;
    static const u08 CC1101_TEST1    =         0x2D;
    static const u08 CC1101_TEST0    =         0x2E;
    /*****************************************************************************/
    // Status registers Can only be read must Add the burst Bit to be able to read
    /*****************************************************************************/
    static const u08 CC1101_PARTNUM  =         (0x30 | BURST_MASK);
    static const u08 CC1101_VERSION  =         (0x31 | BURST_MASK);
    static const u08 CC1101_FREQEST  =         (0x32 | BURST_MASK);
    static const u08 CC1101_LQI      =         (0x33 | BURST_MASK);
    static const u08 CC1101_RSSI     =         (0x34 | BURST_MASK);
    static const u08 CC1101_MARCSTATE =         (0x35 | BURST_MASK);
    static const u08 CC1101_WORTIME1  =         (0x36 | BURST_MASK);
    static const u08 CC1101_WORTIME0  =         (0x37 | BURST_MASK);
    static const u08 CC1101_PKTSTATUS =         (0x38 | BURST_MASK);
    static const u08 CC1101_VCO_VC_DAC =        (0x39 | BURST_MASK);
    static const u08 CC1101_TXBYTES    =       (0x3A | BURST_MASK);
    static const u08 CC1101_RXBYTES    =       (0x3B | BURST_MASK);
    static const u08 CC1101_RCCTRL1_STATUS =   (0x3C | BURST_MASK);
    static const u08 CC1101_RCCTRL0_STATUS =   (0x3D | BURST_MASK);
    /*****************************************************************************/
    // MARC STATUS
    /*****************************************************************************/
    static const u08 MARC_STATE_IDLE       =      0x01;
    static const u08 MARC_STATE_RX         =      0x0D;
    static const u08 MARC_STATE_RXFIFO_OV  =       0x11;
    static const u08 MARC_STATE_TX         =       0x13;
    static const u08 MARC_MARC_STATE_TX_END =      0x14;
    static const u08 MARC_STATE_RXTX_SWITCH =      0x15;
    static const u08 MARC_STATE_TXFIFO_UV   =      0x16;
    /*****************************************************************************/
    // GDOx STATUS
    /*****************************************************************************/
    static const u08 CC1100_GDOx_RX_FIFO    =       0x00;
    static const u08 CC1100_GDOx_RX_FIFO_EOP =       0x01;
    static const u08 CC1100_GDOx_TX_FIFO     =      0x02;
    static const u08 CC1100_GDOx_TX_THR_FULL =      0x03;
    static const u08 CC1100_GDOx_RX_OVER     =      0x04;
    static const u08 CC1100_GDOx_TX_UNDER    =      0x05;
    static const u08 CC1100_GDOx_SYNC_WORD   =      0x06;
    static const u08 CC1100_GDOx_RX_OK       =      0x07;
    static const u08 CC1100_GDOx_PREAMB_OK   =      0x08;
    static const u08 CC1100_GDOx_CCA         =      0x09;
    static const u08 CC1100_GDO_PA_PD        =        27;
    static const u08 CC1100_GDO_LNA_PD       =        28;

  private:
    Cspi *spi;
    Cpin *pinGDO0;
    Cpin *pinGDO2;
    bool setPayloadSize(u08 size);
    void wait_PKTSTATUS(u08 status);
    void wait_MARCSTATE(u08 mode);
    void setupRFStudio(void);
    void setSyncWord(u08* sync);
    void setDevAddress(u08 addr);
    void setChannel(u08 chnl);
    u32 convDB(u08 rssi) {
      if (rssi < 128) {
        return -((rssi / 2) - RSSI_OFFSET);
      } else {
        return -(((rssi - 256) / 2) - RSSI_OFFSET);
      }
    }
    u08 payloadSize;
    u08 rssi;
    u08 lqi;
    u08 channel;
    u08 syncWord[2];
    u08 devAddress;
    etxType txType;
  public:
    u08 *payload;
    bool packetAvailable;
    void rssiValidWait(void);
    Tfifo<u08> rxFifo;
    Tfifo<u08> txFifo;
    CC1101(Cspi *_spi, Cpin *_pinGDO0 ,Cpin *_pinGDO2 , u08 _payloadSize, u08 _addr = 0, u08 _chnl = 0, u08 _rxbuflen = 32, u08 _txbuflen = 8, etxType _txType = eCC1100_TX_TYPE_FORCED);
    bool transmit(u08 *data, u08 len);
    void setRxMode(void);
    void rxISR(void);
    void setTxType(etxType _txType);
    void idle(void);
    void wakeUp(void);
    void reset(void);
    void powerdown();
};
#endif

