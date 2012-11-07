#ifndef CGSM_H
#define CGSM_H

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "common.h"
#include "uart.h"
#include "fifo.h"
#include "scheduler.h"

#define MDM_TIMEOUT_VAL  1000000 //1sec
#define SERVICE_INTERVAL  200000//
#define MDM_RETRIES_VAL 3
#define DEBUG_PRINT

//*****************************************************************************
typedef enum {
  ERR_NONE, ERR_NO_SIMCARD, ERR_PIN_ON_SIMCARD
} eMdmError;
//*****************************************************************************
typedef enum {
  SOCK_CLOSED, SOCK_INIT, SOCK_CLOSE_WAIT, SOCK_ESTABLISHED
} eSocketState;
//*****************************************************************************
typedef enum {
  START = 1, SEND, RETRY,
} eCmdState;
//*****************************************************************************
typedef enum {
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
typedef struct {
    u08 index;
    u08 name[20];
    u08 nameLen;
    u08 number[30];
    u08 numberLen;
    u08 addrType;
} sPhoneBook;
/*****************************************************************************/
typedef struct {
    c08 nr[10];
    c08 phonenum[32];
    c08 timestamp[32];
    c08 message[140];
} sSMS;
//*****************************************************************************
class Cmodem : public Csignal {
  private:
    bool GetAtResp(char* rspStr, c08* rxRsp = NULL);
    void StartupTask(void);
    void ConfigureTask(void);
    bool HandleAtCmd(c08* cmd, const char* _expRsp);
    bool checkSignalStrength();
    bool checkSIM();
    bool checkRegistration();
    bool PowerOff(void);
    bool PowerOn(void);
    void clearTimer(void);
    Csignal timeout;
    static const f32 period = 100e-3;
    static const u08 bufSize = 64;
    static const u08 numRetries = 2;

  public:
    Cmodem(Cuart * _pUart, u08 bufSize = bufSize);
    bool initModem(void);
    bool initIP(bool useDns);
    bool connect(void);
    bool disconnect(void);
    void service(void);
    bool write(u08* dat, u16 len);
    bool write(Tfifo<u08>* dat);
    void ServerSetIP(c08* _IP, c08 *_port, bool _usedns);
    Tfifo<u08> rxFifo;
    eSocketState ss;
    Cuart *pUart;
    char serverIP[32];
    bool usedns;
    char port[8];
    u08 gprsraw[bufSize];
};
#endif
