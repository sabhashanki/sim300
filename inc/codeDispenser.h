/*
 * codeDispenser.h
 *
 *  Created on: 31 Jul 2009
 *      Author: Wouter
 */
#ifndef CODEDISPENSER_H_
#define CODEDISPENSER_H_

#include "encoder.h"


typedef enum {
  CODE_CMD_SERVICE = 1, CODE_CMD_LOADING = 2, CODE_CMD_ACCESS = 3, CODE_CMD_LOCK = 4
} eCodeCommand;

typedef enum {
  DUR_A = 4, DUR_B = 8, DUR_C = 16, DUR_D = 32, DUR_MAX = DUR_D
} eCodeDuration;

class CCodeDispenser {
    u08 cmdIndex;
    u08 durIndex;
    u32 currentTime;
    sCodeRanges codeRanges;
    u32 mask;
    u32 max;
  public:
    Cencoder encService;
    Cencoder encLoading;
    Cencoder encAccess;
    CCodeDispenser();
    void set(u16 vehicle);
    bool isValidCode(u16 vehicle, u32 minFromEpoch, eCodeCommand _cmd, u32 code);
    bool isUsedCode(u32 code);
    u32 getCode(u16 vehicle, u32 minFromEpoch, eCodeCommand cmd, eCodeDuration dur);
};

#endif /* CODEDISPENSER_H_ */
