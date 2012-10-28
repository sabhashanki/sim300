#ifndef RADIOMASTER2LOCK_H
#define RADIOMASTER2LOCK_H
/****************************************************************************************/
#include "CNetwork.h"
/********************************* Opcodes ******************************************/
namespace RM2LOCK {
/****************************************************************************************/
#define STATE_UNKNOWN 0
/****************************************************************************************/
typedef enum {
  GET_BOLT_STATE = 0x01,
  SET_SOLENOID_ON = 0x02,
  SET_SOLENOID_OFF = 0x03,
  GET_SOLENOID_STATE = 0x04,
  GET_BUTTON_STATE = 0x05,
  GET_LED_STATE = 0x06,
  SET_LED_ON = 0x07,
  SET_LED_OFF = 0x08,
  GET_PROX_STATE = 0x09,
  GET_SW_VERSION = 0x0A,
  SET_NODE_ID = 0x0B,
  HELLO = 0x0C,
  BAD_OPCODE = 0x0D,
  DEBUG_INFO = 0x0E
} eLockOpcodes;
/****************************************************************************************/
typedef enum {
  BUTTON_UNKNOWN = STATE_UNKNOWN, BUTTON_PRESSED = 1, BUTTON_RELEASED = 2
} eButtonState;
/****************************************************************************************/
typedef enum {
  SOLENOID_UNKNOWN = STATE_UNKNOWN, SOLENOID_ON = 1, SOLENOID_OFF = 2
} eSolenoidState;
/****************************************************************************************/
typedef enum {
  BOLT_UNKNOWN = STATE_UNKNOWN, BOLT_OPEN = 1, BOLT_CLOSED = 2
} eBoltState;
/****************************************************************************************/
typedef enum {
  LED_UNKNOWN = STATE_UNKNOWN, LED_ON = 1, LED_OFF = 2
} eLedState;
/****************************************************************************************/
typedef enum {
  PROX_UNKNOWN = 0, PROX_PRESENT = 1, PROX_REMOVED = 2
} eProxState;
/****************************************************************************************/
typedef struct {
  u08 Opcode;
  u08 Data;
} sSafeCmd;
/****************************************************************************************/
typedef struct {
  CNETWORK::sHeader Header;
  sSafeCmd Cmd;
} sSafePkt;
}
#endif
