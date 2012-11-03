#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
/****************************************************************************************/
#include "cc1101.h"
#include "rfstudio.h"
#include "crc.h"
#include "pin.h"

#include "common.h"

#define TRANSMIT

#define STATE_IDLE    0
#define STATE_RX      1
#define STATE_TX      2
#define STATE_FSTXON  3
#define STATE_CALIB   4
#define STATE_SETTL   5
#define STATE_RXOVER  6
#define STATE_TXUNDER 7

/**************************************************************************************
 * PATABLE
 ***************************************************************************************/
const u08 paTable[8] = { 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0 };
/****************************************************************************************/
static CC1101* pCC1101;
/****************************************************************************************/
CC1101::CC1101(Cspi *_spi, Cpin *_pinGDO0, Cpin *_pinGDO2, u08 _payloadSize, u08 _addr,
               u08 _chnl, u08 _rxbuflen, u08 _txbuflen, etxType _txType) {

  spi = _spi;
  pCC1101 = this;
  pinGDO0 = _pinGDO0;
  pinGDO2 = _pinGDO2;

  setPayloadSize(_payloadSize);
  rxFifo.setBufSize(_rxbuflen);
  txFifo.setBufSize(_txbuflen);

  reset();
  setupRFStudio();

  if (_addr) {
    setDevAddress(_addr);
  }

  if (_chnl) {
    setChannel(_chnl);
  }

  txType = _txType;

  rssiValidWait();

  //The falling edge on PCINT6 ==> GDO 0
  EICRA = _BV(ISC01);
  PCMSK0 |= (1 << PCINT6); //En
  PCICR |= PCIE0;
}

/****************************************************************************************/
bool CC1101::setPayloadSize(u08 size) {
  payload = (u08*) malloc(size);
  if (payload == NULL) {
    return false;
  }
  payloadSize = size;
  return true;
}

/*****************************************************************************
 * rssi_valid_wait
 ****************************************************************************/
void CC1101::idle(void) {
  switch (((spi->strobe(CC1101_SNOP) >> 4) & 0x7)) {
    case STATE_RXOVER:
      spi->strobe(CC1101_SRX);
      break;
    case STATE_TXUNDER:
      spi->strobe(CC1101_STX);
      break;
    default:
      spi->strobe(CC1101_SIDLE);
      break;
  }
  while (((spi->strobe(CC1101_SNOP) >> 4) & 0x7) != STATE_IDLE)
    ;
}

/*****************************************************************************
 * rssi_valid_wait
 ****************************************************************************/
void CC1101::rssiValidWait(void) {
  u08 delay;
  u08 stat;
  delay = 0;
  do {
    stat = spi->read(CC1101_PKTSTATUS);
    if (stat & 0x50) {
      break;
    }
    _delay_us(10);
    delay++;
  } while (delay < 200);
}

/****************************************************************************
 * transmit data
 ****************************************************************************/
bool CC1101::transmit(u08 *data, u08 len) {
#ifdef TRANSMIT
  u08 cca, cca_cnt, mode;
  cca = 0;
  mode = 0;
  u16 crc;
  /* ==============================================================================
   FORCED TRANSMIT
   ================================================================================*/
  switch (txType) {
    case eCC1100_TX_TYPE_FORCED:
      //while (pinGDO0->isHI());
      crc = crc16(data, len, 0xCC);
      spi->write(CC1101_TXFIFO, (len + 2)); // Set data len at the first position of the TX FIFO +2 for crc's
      spi->write(CC1101_TXFIFO | BURST_MASK, data, len); // Write packet data
      spi->write(CC1101_TXFIFO, (crc >> 8) & 0xFF);
      spi->write(CC1101_TXFIFO, (crc) & 0xFF);
      spi->strobe(CC1101_STX); // Start Transmission
      //while (pinGDO0->isHI());
      cca_cnt = 0;
      do {
        mode = spi->read(CC1101_MARCSTATE) & 0x1F;
        cca_cnt++;
        _delay_us(100);
        if (cca_cnt > 10) {
          idle();
          _delay_us(200);
          return false;
        }
      } while ((mode != MARC_STATE_IDLE)); // TX to IDle with calibration - 725 us
      break;

    case eCC1100_TX_TYPE_CCA:
#if 0
      // url:http://e2e.ti.com/support/low_power_rf/w/design_notes/implementation-of-cca-in-an-application-cc1100-cc1101-cc1110-cc1111-cc2500-cc2510-cc2511.aspx
      // http://e2e.ti.com/support/low_power_rf/f/155/t/169134.aspx

      //The current CCA state is viewable on GD0
      //================= Send Packet to FIFO ==================
      crc = crc16(data, len, 0xCC);
      spi->write(CC1101_TXFIFO, len + 2);
      spi->write(CC1101_TXFIFO | BURST_MASK, data, len);
      spi->write(CC1101_TXFIFO, (crc >> 8) & 0xFF);
      spi->write(CC1101_TXFIFO, (crc) & 0xFF);
      // Seed the randomizer
      srand(crc);
      cca_cnt = 0;
      while (1) {
        // ================= SET RX MODE==========================
        spi->strobe(CC1101_SRX);
        wait_MARCSTATE(MARC_STATE_RX);
        // Give time for the RSSI levels to be sampled
        _delay_us(500);
        // ================= SET TX MODE==========================
        spi->strobe(CC1101_STX);
        // Datasheet RX->TX switch (~30us).
        _delay_us(50);
        mode = spi->read(CC1101_MARCSTATE) & 0x1F;
        //if(mode == MARC_STATE_TX || cca_cnt > 5 || BIT_IS_LO(READ_GD0,PIN_GD0)){
        if (mode == MARC_STATE_TX) {
          return true;
        }
        if (cca_cnt > 10) {
          idle();
          return false;
        }
        cca_cnt++;
        cca = rand() & 0xFF;
        _delay_us(cca);
      }
      // TX to IDle with calibration - 725 us
      wait_MARCSTATE(MARC_STATE_IDLE);
      return true;
#endif
      break;

  }
#endif
  return true;
}

void CC1101::setRxMode(void) {
// ================= SET RX MODE==========================
  spi->strobe(CC1101_SRX);
  wait_MARCSTATE(MARC_STATE_RX);
  _delay_us(500);
}

void CC1101::rxISR(void) {
  u08 len;
  u08 rxbytes;
  u08 rxchk;
  u08 state;
  u08 pcktStatus;
  u16 rxCrc;
  u16 crc;

  // Test for GDO0 line to go low
  pcktStatus = spi->read(CC1101_PKTSTATUS);
  rxchk = spi->read(CC1101_RXBYTES); // Only length of payload will be 2 more than payloadSize 2 crc's
  state = spi->read(CC1101_MARCSTATE);

  do {
    rxbytes = rxchk;
    rxchk = spi->read(CC1101_RXBYTES);
  } while (rxchk != rxbytes);

  if (rxbytes == 0) {
    return;
  }
  if ((rxbytes >= payloadSize) && ((pcktStatus & 0x01) == 0x00)) {
    len = spi->read(CC1101_RXFIFO); // First byte is the len
    if (len > (payloadSize + 2)) {
      spi->strobe(CC1101_SIDLE);
      _delay_us(100);
      spi->strobe(CC1101_SFRX); // Flush RX FIFO - > OVERLFOW
      packetAvailable = false;
      spi->strobe(CC1101_SRX);
      return;
    }
    spi->read(CC1101_RXFIFO | BURST_MASK, payload, payloadSize);
    //while (pinGDO0->isHI()) ;
    rxCrc = spi->read(CC1101_RXFIFO);
    rxCrc = (rxCrc << 8) + spi->read(CC1101_RXFIFO);
    crc = crc16(payload, payloadSize, 0xCC);
    if (1) { //crc == rxCrc) {
      rssi = convDB(spi->read(CC1101_RXFIFO));
      lqi = spi->read(CC1101_RXFIFO);
      //crc_ok = (spi->read(CC1101_PKTSTATUS)) & _BV(7);
      packetAvailable = true;
      rxFifo.write(payload, payloadSize);
    } else {
      spi->strobe(CC1101_SFRX); // Flush RX FIFO
    }
  }
}

//*****************************************************************************
void CC1101::wait_PKTSTATUS(u08 status) {
  while (1) {
    if ((spi->read(CC1101_PKTSTATUS) & 0x1F) == status) {
      break;
    }
    _delay_us(10);
  }
}

//*****************************************************************************
void CC1101::wait_MARCSTATE(u08 mode) {
  while (1) {
    if ((spi->read(CC1101_MARCSTATE) & 0x1F) == mode) {
      break;
    }
    _delay_us(10);
  }
}
/*****************************************************************************
 * Reset CC1101
 ****************************************************************************/
void CC1101::reset(void) {
  spi->pinCS->disable();
  _delay_us(5);
  spi->pinCS->enable();
  _delay_us(10);
  spi->pinCS->disable();
  _delay_us(41);
  spi->pinCS->enable();

  spi->wait();
  spi->send(CC1101_SRES);
  spi->wait();
  spi->pinCS->disable();

}

/********** **********************************************************************
 * Configure CC1101 registers according to RF DESIGN STUDIO
 *******************************************************************************/
void CC1101::setupRFStudio(void) {
  /*****************************************************************************/
  spi->write(CC1101_IOCFG2, CC1101_DEFVAL_IOCFG2); //GDO2 Output Pin Configuration
  spi->write(CC1101_IOCFG1, CC1101_DEFVAL_IOCFG1); // GDO1 Output Pin Configuration
  spi->write(CC1101_IOCFG0, CC1101_DEFVAL_IOCFG0); // GDO0 Output Pin Configuration
  spi->write(CC1101_FIFOTHR, CC1101_DEFVAL_FIFOTHR); // RX FIFO and TX FIFO Thresholds

  /*****************************************************************************/
  spi->write(CC1101_SYNC1, CC1101_DEFVAL_SYNC1); // Sync Word, High Byte
  spi->write(CC1101_SYNC0, CC1101_DEFVAL_SYNC0); // Sync Word, Low Byte
  /*****************************************************************************/
  spi->write(CC1101_PKTLEN, CC1101_DEFVAL_PKTLEN);
  spi->write(CC1101_PKTCTRL1, CC1101_DEFVAL_PKTCTRL1);
  spi->write(CC1101_PKTCTRL0, CC1101_DEFVAL_PKTCTRL0);
  /*****************************************************************************/
  spi->write(CC1101_ADDR, CC1101_DEFVAL_ADDR); // Device Address
  spi->write(CC1101_CHANNR, CC1101_DEFVAL_CHANNR); // Channel Number
  /*****************************************************************************/
  spi->write(CC1101_FSCTRL1, CC1101_DEFVAL_FSCTRL1); //Frequency Synthesizer Control
  spi->write(CC1101_FSCTRL0, CC1101_DEFVAL_FSCTRL0);
  /*****************************************************************************/
  spi->write(CC1101_FREQ2, CC1101_DEFVAL_FREQ2); // Frequency Control
  spi->write(CC1101_FREQ1, CC1101_DEFVAL_FREQ1);
  spi->write(CC1101_FREQ0, CC1101_DEFVAL_FREQ0);
  /*****************************************************************************/
  spi->write(CC1101_MDMCFG4, CC1101_DEFVAL_MDMCFG4); // Modem Configuration
  spi->write(CC1101_MDMCFG3, CC1101_DEFVAL_MDMCFG3);
  spi->write(CC1101_MDMCFG2, CC1101_DEFVAL_MDMCFG2);
  spi->write(CC1101_MDMCFG1, CC1101_DEFVAL_MDMCFG1);
  spi->write(CC1101_MDMCFG0, CC1101_DEFVAL_MDMCFG0);
  /*****************************************************************************/
  spi->write(CC1101_DEVIATN, CC1101_DEFVAL_DEVIATN); // Modem Deviation Setting
  /*****************************************************************************/
  spi->write(CC1101_MCSM2, CC1101_DEFVAL_MCSM2); //Main Radio Control State Machine
  spi->write(CC1101_MCSM1, CC1101_DEFVAL_MCSM1);
  spi->write(CC1101_MCSM0, CC1101_DEFVAL_MCSM0);
  /*****************************************************************************/
  spi->write(CC1101_FOCCFG, CC1101_DEFVAL_FOCCFG);
  spi->write(CC1101_BSCFG, CC1101_DEFVAL_BSCFG);
  /*****************************************************************************/
  spi->write(CC1101_AGCCTRL2, CC1101_DEFVAL_AGCCTRL2);
  spi->write(CC1101_AGCCTRL1, CC1101_DEFVAL_AGCCTRL1);
  spi->write(CC1101_AGCCTRL0, CC1101_DEFVAL_AGCCTRL0);
  /*****************************************************************************/
  spi->write(CC1101_WOREVT1, CC1101_DEFVAL_WOREVT1);
  spi->write(CC1101_WOREVT0, CC1101_DEFVAL_WOREVT0);
  spi->write(CC1101_WORCTRL, CC1101_DEFVAL_WORCTRL);
  /*****************************************************************************/
  spi->write(CC1101_FREND1, CC1101_DEFVAL_FREND1);
  spi->write(CC1101_FREND0, CC1101_DEFVAL_FREND0);
  /*****************************************************************************/
  spi->write(CC1101_FSCAL3, CC1101_DEFVAL_FSCAL3);
  spi->write(CC1101_FSCAL2, CC1101_DEFVAL_FSCAL2);
  spi->write(CC1101_FSCAL1, CC1101_DEFVAL_FSCAL1);
  spi->write(CC1101_FSCAL0, CC1101_DEFVAL_FSCAL0);
  /*****************************************************************************/
  spi->write(CC1101_RCCTRL1, CC1101_DEFVAL_RCCTRL1);
  spi->write(CC1101_RCCTRL0, CC1101_DEFVAL_RCCTRL0);
  /*****************************************************************************/
  spi->write(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
  spi->write(CC1101_PTEST, CC1101_DEFVAL_PTEST);
  spi->write(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
  /*****************************************************************************/
  spi->write(CC1101_TEST2, CC1101_DEFVAL_TEST2);
  spi->write(CC1101_TEST1, CC1101_DEFVAL_TEST1);
  spi->write(CC1101_TEST0, CC1101_DEFVAL_TEST0);
  /*****************************************************************************/
  spi->write(CC1101_PATABLE | BURST_MASK, (u08*) paTable, 8);
}

/*****************************************************************************
 * 'sync' Synchronization word
 *****************************************************************************/
void CC1101::setSyncWord(u08 *sync) {
#ifdef COMPLETE
  if ((syncWord[0] != sync[0]) || (syncWord[1] != sync[1])) {
    spi->write(CC1101_SYNC1, sync[1]);
    spi->write(CC1101_SYNC0, sync[0]);
    memcpy(syncWord, sync, sizeof(syncWord));
  }
#endif
}

/*****************************************************************************
 * 'addr'	Device address
 *****************************************************************************/
void CC1101::setDevAddress(u08 addr) {
#ifdef COMPLETE
  if (devAddress != addr) {
    spi->write(CC1101_ADDR, addr);
    devAddress = addr;
  }
#endif
}
/*****************************************************************************
 * Set frequency channel
 * 'chnl'  Frequency channel
 * 'save' If TRUE, save parameter in EEPROM
 *****************************************************************************/
void CC1101::setChannel(u08 chnl) {
#ifdef COMPLETE
  if (channel != chnl) {
    spi->write(CC1101_CHANNR, chnl);
    channel = chnl;
  }
#endif
}
/*****************************************************************************
 * Put CC1101 u08o power-down state
 *****************************************************************************/
void CC1101::powerdown() {
#ifdef COMPLETE
  spi->strobe(CC1101_SIDLE);
  spi->strobe(CC1101_SPWD);
#endif
}
/******************************************************************************
 Wakeup from power down State
 ******************************************************************************/
void CC1101::wakeUp(void) {
#ifdef COMPLETE
  spi->pinSS->setHI();
  while (spi->pinMISO->isLO()) {}; /* wait for MISO to go high indicating the oscillator is stable */
  spi->pinSS->setLO();

  /*****************************************************************************/
  spi->write(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
  spi->write(CC1101_PTEST, CC1101_DEFVAL_PTEST);
  spi->write(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
  /*****************************************************************************/
  spi->write(CC1101_TEST2, CC1101_DEFVAL_TEST2);
  spi->write(CC1101_TEST1, CC1101_DEFVAL_TEST1);
  spi->write(CC1101_TEST0, CC1101_DEFVAL_TEST0);
  idle();
#endif
}

/*****************************************************************************
 * Put CC1101 u08o power-down state
 *****************************************************************************/

ISR(SIG_PIN_CHANGE0) {
  BIT_TOGGLE(PORTD, 5);
  pCC1101->rxISR();
}

