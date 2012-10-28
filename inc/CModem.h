#ifndef CGSM_H
#define CGSM_H

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "common.h"
#include "CUART.h"


//namespace CMODEM
//{
#define MDM_TIMEOUT_VAL  1000000 //1sec
#define SERVICE_INTERVAL  200000//
#define MDM_RETRIES_VAL 3
#define MDM_MAX_TX_CMD_LEN  254
#define MDM_MAX_RX_CMD_LEN  254


  //*****************************************************************************
  typedef enum
  {
    START = 1,
    SEND,
    RETRY,
  } eCmdState;
  //*****************************************************************************
  typedef enum
  {
    MDM_UNCONNECTED = 3,
    MDM_STARTUP,
    MDM_CONFIGURE,
    MDM_WAIT_CALL_READY,
    MDM_INIT_OK,
    MDM_SETUP,
    MDM_ERROR,
    MDM_READY,
    MDM_BUSY,
    MDM_GET_SIMCARD_STATUS,
    MDM_SET_SIMCARD_PIN,
    MDM_SIMSWAP,
    MDM_CONNECTED,
    MDM_SEND_SMS,
    MDM_DELETE_SMS,
    MDM_GET_SIGNAL_STRENGHT,
    MDM_GET_SERIAL_NUMBER,
    MDM_SET_ECHO_OFF,
    MDM_SET_SMS_TEXT_MODE,
    MDM_ERROR_CODES,
    MDM_IS_REGISTERED,
    MDM_TEST_AT,
    MDM_GET_TIME,
    MDM_ADD_IPHEAD,
    MDM_GET_SMS,
    MDM_ECHO_OFF,
    MDM_ERROR_REPORT,
    MDM_EXTENDED_ERROR,
    MDM_TEST_SIMRDY,
    MDM_READ_SMS,
    MDM_SMS_WAIT_SEND_OK,
    MDM_WAIT_OK,
    MDM_RESET,
    MDM_CONNECT_TO_SERVER,
    MDM_SET_DNS_CONNECT,
    MDM_SET_DNS_CONFIG,
    MDM_CONNECTED_TO_SERVER,
    MDM_IP_SEND,
    MDM_IPSEND_VERIFY,
    MDM_IPSHUTDOWN,
    MDM_IP_STATUS,
    MDM_GET_BALANCE,
    MDM_SET_GPRS_MODE,
    MDM_SET_SHOW_REMOTE_IP,
    MDM_ACTIVATE_PDP,
    MDM_SET_TCP_PORT,
    MDM_START_IP,
    MDM_WAIT_IP_CONNECT,
    MDM_START_TCP_TASK,
    MDM_GET_IP,
    MDM_FAILED
  } eMdmState;
  /*****************************************************************************/
  typedef struct
  {
    u08 index;
    u08 name[20];
    u08 nameLen;
    u08 number[30];
    u08 numberLen;
    u08 addrType;
  } sPhoneBook;
  /*****************************************************************************/
  typedef struct
  {
    c08 nr[10];
    c08 phonenum[32];
    c08 timestamp[32];
    c08 message[140];
  } sSMS;
  //*****************************************************************************
  class CModem
  {
    private:

    eMdmState mdmState;
    eCmdState cmdState;
    eMdmState failState;
    volatile u32 atomicTime;
    volatile u32 serviceTime;
    u32 timeout;
    c08 txcmd[MDM_MAX_TX_CMD_LEN];
    u16 txlen;
    c08 rxmsg[MDM_MAX_RX_CMD_LEN];
    u16 rxlen;
    u08 rxindex;
    u08 retry;
    u08 taskRetry;
    u08 signalStrenght;
    char socket[8];

    c08 pincode[6];
    u08 debugLevel;
    u08 pwrState;
    u08 initState;
    c08 *str;
    c08 *pRx;

    u08 error_cnt;
    bool GetAtResp(PGM_P rspStr);

    void StartupTask(void);
    void ConfigureTask(void);
    bool Init(void);
    //bool (CModem::*Callback)(eMdmState, eMdmState,PGM_P);
    bool HandleAtCmd(c08 *str, PGM_P rspStr);
    //void SetCallback( u08 (CModem::*Task)(u08,u08,PGM_P));
    bool PowerOff(void);
    bool PowerOn(void);
    void clearTimer(void);
  public:
    CUART *pUart;
    u08 simcard_ok;
    u08 signal_ok;
    u08 connect_ok;
    u08 registered_ok;
    char serverIP[32];
    bool usedns;
    char port[8];
    volatile u32 isr_timer;
    sSMS sms;
    u08 smsrx;
    u08 smstx_en;
    c08 gprsraw[MDM_MAX_RX_CMD_LEN];
    u08 gprsrx;
    CModem(CUART * _pUart);
    void Service(void);
    bool SIMCheckReady(void);
    void UpdateMdmStatus(void);
    bool SIMSetPin(c08* pin);
    void setNextState(eMdmState nextState);
    eMdmState GetStateModem(void);
    bool GetSignalQuality(void);
    bool SendSMS(char *PhoneNumber, char *Message);
    bool DataToServer(c08* dat);
    void ServerSetIP(c08* _IP,c08 *_port,bool _usedns);
    void GetUnSolicited(void);
  };
#endif
