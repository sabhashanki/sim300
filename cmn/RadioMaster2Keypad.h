#ifndef RADIOMASTER2KEYPAD_H
#define RADIOMASTER2KEYPAD_H

#include "avrlibtypes.h"
/********************************* KEYPAD LCD Opcodes ***********************************/
namespace RM2KP {
  typedef enum {
    CLEAR_LCD = 0x01,
    SET_LCD_STRING = 0x02,
    CLEAR_KEYPAD = 0x03,
    GET_KEYPAD_KEY = 0x04,
    GET_KEYPAD_DECIMAL = 0x05,
    GET_SW_VERSION = 0x06,
    HELLO = 0x07,
    DEBUG_INFO = 0x08,
    BAD_OPCODE = 0x09,
    GET_DISCRETES = 0x0A,
    GET_DISCRETE = 0x0B,
    SET_DISCRETE_ON = 0x0C,
    SET_DISCRETE_OFF = 0x0D
  } eKeyPadOpcodes;

  typedef enum {
    OUT_0 = 0,
    OUT_1 = 1,
    IN_0 = 2,
    IN_1 = 3,
    LED0 = 4,
    BACKUP = 5,
    BACKLIGHT = 6,
  } eDiscreteNum;

  typedef struct {
      eDiscreteNum Number;
  } sDiscrete;
  /****************************************************************************************
   COMMAND STRUCTURES
   ****************************************************************************************/
  typedef struct {
      u08 StrLen;
      u08 xPos;
      u08 yPos;
      u08 Str[80];
  } sKeypadDisplayTextCmd;
  /****************************************************************************************/
  typedef struct {
      u08 Len;
  } sKeypadGetDigitsCmd;
  /****************************************************************************************/
  typedef union {
      sKeypadDisplayTextCmd DisplayText;
      sKeypadGetDigitsCmd GetDigits;
      sDiscrete Discrete;
  } uKeypadCmd;
  /****************************************************************************************/
  typedef struct {
    eKeyPadOpcodes Opcode;
      uKeypadCmd Dat;
  } sKeypadCmd;
  /***************************	RESPONSE STRUCTURES *****************************************************/
  typedef union {
      u32 Digits;
      u08 Key;
      u08 SwVer;
  } uKeypadResp;
  /****************************************************************************************/
  typedef struct {
      eKeyPadOpcodes Opcode;
      bool Result;
  } sKeypadRespHeader;
  /****************************************************************************************/
  typedef struct {
      sKeypadRespHeader Hdr;
      uKeypadResp Dat;
  } sKeypadResp;
}

#endif
