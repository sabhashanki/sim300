/*
 * codeDispenser.h
 *
 *  Created on: 31 Jul 2009
 *      Author: Wouter
 */
#ifndef OPERATORCODER_H_
#define OPERATORCODER_H_

#include "encoder.h"

class COperatorCoder {
    u08 cmdIndex;
    u08 durIndex;
    u32 currentTime;
    sCodeRanges codeRanges;
    u32 mask;
  public:
    Cencoder encoder;
    COperatorCoder();
    void set(u16 vehicle);
    bool isValidCode(u32 minFromEpoch, u32 code);
    bool isUsedCode(u32 code);
    u32 getCode(u32 minFromEpoch);
};

#endif /* CODEDISPENSER_H_ */
