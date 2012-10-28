/*
 * Cencoder.h
 *
 *  Created on: 30 Oct 2009
 *      Author: Wouter
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "avrlibtypes.h"

typedef struct {
  u32 startIndex;
  u08 numCodes;
} sCodeRange;

typedef struct {
  u08 numRanges;
  sCodeRange range[4];
} sCodeRanges;

class Cencoder {
  u32 seed;
  u32 mask;
  u32 max;
  public:
    Cencoder(void);
    Cencoder(u32 seed, u32 mask, u32 max);
    void set(u32 seed, u32 mask, u32 max);
    u32 getCode(u32 index);
    void getCodes(u32* codes, sCodeRanges* ranges);
    u32 getNextCode(u32 prevCode);
};

#endif /* CENCODER_H_ */
