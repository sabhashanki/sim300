/*
 * IIOModule.h
 *
 *  Created on: 09 Mar 2009
 *      Author: Wouter
 */

#ifndef IIOMODULE_H_
#define IIOMODULE_H_

#include "avrlibtypes.h"

namespace IIOMODULE {
  /****************************************************************************************/
  typedef enum {
    OFF = 0, ON = 1
  } eDiscreteValue;
  /****************************************************************************************/
  typedef enum {
    IN_OUT_0 = 2,
    IN_OUT_1 = 3,
    IN_OUT_2 = 4,
    IN_OUT_3 = 5,
    LED0 = 6,
    LED1 = 7,
    BUZZER = 8,
    FP_PWR = 9,
    FP_LED0 = 10,
    FP_LED1 = 11,
    FP_INPUT = 12
  } eDiscreteNum;
  /********************************* Opcodes ******************************************/
  typedef enum {
    GET_DISCRETES = 10,
    GET_DISCRETE = 20,
    SET_DISCRETE_ON = 30,
    SET_DISCRETE_OFF = 40,
    OPCODE_UNKNOWN = 50,
    OPCODE_ERROR = 60,
    OPCODE_HELLO = 70,
    SET_DATE = 80,
    GET_DATE = 90,
    GET_MINUTES_FROM_EPOCH = 100,
    GET_SW_VERSION = 110
  } eOpcodes;

  /****************************************************************************************/
  typedef struct {
    u16 year;
    u08 mon;
    u08 day;
    u08 hour;
    u08 min;
  } sDate;
  /**************************** COMMAND STRUCTURES**************************************/
  typedef struct {
      eDiscreteNum Number;
  } sDiscrete;
  /****************************************************************************************/
  typedef union {
      sDiscrete Discrete;
      sDate Date;
  } uDiscrete;
  /****************************************************************************************/
  typedef struct {
      u08 Opcode;
      uDiscrete Dat;
  } sCmd;
  /**************************** RESPONSE STRUCTURES ***************************************/
  typedef struct {
    eDiscreteValue valInOut0;
    eDiscreteValue valInOut1;
    eDiscreteValue valInOut2;
    eDiscreteValue valInOut3;
    eDiscreteValue valLed0;
    eDiscreteValue valLed1;
    eDiscreteValue valBuzzer;
    eDiscreteValue valFPLed0;
    eDiscreteValue valFPLed1;
    eDiscreteValue valFPInput;
    eDiscreteValue valFPPwr;
  } sDiscretes;

  /****************************************************************************************/
  typedef union {
      eDiscreteValue Discrete;
      sDiscretes     Discretes;
      u32 Minutes;
      sDate Date;
  } uResp;
  /****************************************************************************************/
  typedef struct {
      eOpcodes Opcode;
      uResp Dat;
  } sResp;
  /****************************************************************************************/
  class IIOModule {
    public:
      virtual bool getDiscretes(sDiscretes* discValues) = 0;
      virtual bool getDiscrete(eDiscreteValue* discValue,
          eDiscreteNum discreteNum) = 0;
      virtual bool setDiscreteOn(eDiscreteNum discreteNum) = 0;
      virtual bool setDiscreteOff(eDiscreteNum discreteNum) = 0;
  };
}
#endif /* IIOMODULE_H_ */
