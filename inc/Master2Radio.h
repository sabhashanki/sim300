#ifndef MASTER2RADIO_H
#define MASTER2RADIO_H
/****************************************************************************************/
#include "CNetwork.h"
/****************************************************************************************/
using namespace CNETWORK;
/********************************* Opcodes ******************************************/
namespace MASTER2RADIO {
#define STATE_UNKNOWN 0
  /****************************************************************************************/
  typedef enum {
    SET_NODE_ID = 1,
      HELLO = 2,
      BAD_OPCODE = 3,
      DEBUG_INFO = 4,
      GET_SW_VERSION = 5,
      SET_DATE = 6,
      GET_DATE = 7,
      GET_MINUTES_FROM_EPOCH = 8 ,
      GET_PINS_STATE = 9,
      SET_PIN_STATE = 10,
      DISCONNECT = 11,
      CONNECT = 12
  } eOpcodes;
  /****************************************************************************************/
  typedef enum {
    PIN_0 = 0, PIN_1 = 1, PIN_2 = 2, PIN_3 = 3, BUZZER_0 = 4, RELAY_0 = 5
  } ePinNum;
  /****************************************************************************************/
  typedef enum {
    PIN_UNKNOWN = STATE_UNKNOWN, PIN_HIGH = 1, PIN_LOW = 2
  } ePinState;
  /****************************************************************************************/
    typedef struct {
      u16 year;
      u08 mon;
      u08 day;
      u08 hour;
      u08 min;
  } sDate;

  /****************************************************************************************/
  typedef struct {
      ePinNum pinNum;
      ePinState pinState;
  } sSetPinState;
  /****************************************************************************************/
  typedef struct {
      sDate Date;
      sSetPinState Pin;
  } uCmd;
  /****************************************************************************************/
  typedef struct {
      eOpcodes Opcode;
      uCmd Dat;
  } sCmd;
  /***************************  RESPONSE STRUCTURES *****************************************************/
  typedef struct {
      ePinState statePin0;
      ePinState statePin1;
      ePinState statePin2;
      ePinState statePin3;
  } sPinsState;
  /****************************************************************************************/
  typedef union {
      sPinsState PinState;
      u08 SwVer;
      u08 DebugInfo;
      u08 Dat;
      u32 Minutes;
      sDate Date;
  } uRespDat;
  /****************************************************************************************/
  typedef struct {
      eOpcodes Opcode;
      uRespDat Data;
  } sResp;
/****************************************************************************************/
}
#endif
