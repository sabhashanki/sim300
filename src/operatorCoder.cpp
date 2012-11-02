/*
 * codeDispenser.cpp
 *
 *  Created on: 31 Jul 2009
 *      Author: Wouter
 */

#ifndef WIN32
#include <util/atomic.h>
#include <avr/eeprom.h>
#endif

#include "operatorCoder.h"

#define NUM_CODES   168     // 7 days using an hour tick

#ifdef SBV
#define START_INDEX 100
#endif
#ifdef COIN
#define START_INDEX 200
#endif

COperatorCoder::COperatorCoder() {
 u32 mask;
 u32 max;
 // Taps for a maximal 32-bit LFSR : 32; 30; 26; 25
 mask = ((u32) 0x1 << (32-1)) | ((u32) 0x1 << (30-1)) | ((u32) 0x1 << (26-1)) | ((u32) 0x1 << (25-1));
 max = 0xFFFFFFFF;
 encoder.set(START_INDEX, mask, max);
 codeRanges.numRanges = 1;
 codeRanges.range[0].numCodes = NUM_CODES;
}

bool COperatorCoder::isValidCode(u32 hoursFromEpoch, u32 code) {
  u32 codes[NUM_CODES];
  u08 cnt;

 codeRanges.numRanges = 1;
 codeRanges.range[0].numCodes = NUM_CODES;
  codeRanges.range[0].startIndex = START_INDEX + hoursFromEpoch;
  encoder.getCodes(codes, &codeRanges);
  for (cnt = 0; cnt < NUM_CODES; cnt++) {
    if (codes[cnt] == code) {
      return true;
    }
  }
  return false;
}

u32 COperatorCoder::getCode(u32 hoursFromEpoch) {
  u32 codes[NUM_CODES];

 codeRanges.numRanges = 1;
 codeRanges.range[0].numCodes = NUM_CODES;
  codeRanges.range[0].startIndex = START_INDEX + hoursFromEpoch;
  encoder.getCodes(codes, &codeRanges);
  return codes[NUM_CODES - 1];
}

bool COperatorCoder::isUsedCode(u32 code) {
  u08 cnt;
  u32* pCodes;
  u32 tmp;
  bool result = false;

#ifndef WIN32
  pCodes = (u32*) CODES_BASE;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    for (cnt = 0; cnt < NUM_CODES_SAVED; cnt++) {
      if (eeprom_read_dword(pCodes) == code) {
        result = true;
      }
      pCodes++;
    }
    if (!result) {
      // Code not found write the code into the table
      pCodes = (u32*) CODES_BASE;
      for (cnt = 0; cnt < NUM_CODES_SAVED; cnt++) {
        tmp = eeprom_read_dword(pCodes);
        if (tmp == 0xFFFFFFFF) {
          eeprom_write_dword(pCodes, code);
          if (cnt < (NUM_CODES_SAVED - 1)) {
            pCodes++;
            eeprom_write_dword(pCodes, 0xFFFFFFFF);
            return result;
          } else {
            pCodes = (u32*) CODES_BASE;
            eeprom_write_dword(pCodes, 0xFFFFFFFF);
          }
        }
        pCodes++;
      }
    }
  }
#endif
  return result;
}

void COperatorCoder::set(u16 vehicle) {
#ifdef SBV
#endif
#ifdef COIN
#endif
}

