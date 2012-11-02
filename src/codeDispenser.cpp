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

#include "codeDispenser.h"

#define NUM_CODES_SAVED   ((DUR_A + DUR_B + DUR_C + DUR_D)*2)
#define CODES_BASE        8

#ifdef SBV
#define START_INDEX 1000
#endif
#ifdef COIN
#define START_INDEX 10000
#endif
#define DUR_SEPERATION 1000

CCodeDispenser::CCodeDispenser() {
  mask = ((u32) 0x1 << 19) | ((u32) 0x1 << 18) | ((u32) 0x1 << 15) | ((u32) 0x1 << 13);
  max = 999999;
  codeRanges.numRanges = 4;
  codeRanges.range[0].numCodes = DUR_A;
  codeRanges.range[1].numCodes = DUR_B;
  codeRanges.range[2].numCodes = DUR_C;
  codeRanges.range[3].numCodes = DUR_D;
}

bool CCodeDispenser::isValidCode(u16 vehicle, u32 minFromEpoch, eCodeCommand cmd,
                                 u32 code) {
  u32 codes[DUR_A + DUR_B + DUR_C + DUR_D];
  u08 cnt;

  set(vehicle);

  codeRanges.range[0].startIndex = (START_INDEX) + minFromEpoch;
  codeRanges.range[1].startIndex = (START_INDEX + DUR_SEPERATION) + minFromEpoch;
  codeRanges.range[2].startIndex = (START_INDEX + 2*DUR_SEPERATION) + minFromEpoch;
  codeRanges.range[3].startIndex = (START_INDEX + 3*DUR_SEPERATION) + minFromEpoch;

  switch (cmd) {
    case CODE_CMD_SERVICE:
      encService.getCodes(codes, &codeRanges);
      break;
    case CODE_CMD_LOADING:
      encLoading.getCodes(codes, &codeRanges);
      break;
    case CODE_CMD_ACCESS:
      encAccess.getCodes(codes, &codeRanges);
      break;
    default:
      return false;
  }
  for (cnt = 0; cnt < (DUR_A + DUR_B + DUR_C + DUR_D); cnt++) {
    if (codes[cnt] == code) {
      return true;
    }
  }
  return false;
}

u32 CCodeDispenser::getCode(u16 vehicle, u32 minFromEpoch, eCodeCommand cmd,
                            eCodeDuration dur) {
  u32 codes[DUR_A + DUR_B + DUR_C + DUR_D];

  set(vehicle);

  codeRanges.range[0].startIndex = (START_INDEX) + minFromEpoch;
  codeRanges.range[1].startIndex = (START_INDEX + DUR_SEPERATION) + minFromEpoch;
  codeRanges.range[2].startIndex = (START_INDEX + 2*DUR_SEPERATION) + minFromEpoch;
  codeRanges.range[3].startIndex = (START_INDEX + 3*DUR_SEPERATION) + minFromEpoch;

  switch (cmd) {
    case CODE_CMD_SERVICE:
      encService.getCodes(codes, &codeRanges);
      break;
    case CODE_CMD_LOADING:
      encLoading.getCodes(codes, &codeRanges);
      break;
    case CODE_CMD_ACCESS:
      encAccess.getCodes(codes, &codeRanges);
      break;
    default:
      return false;
  }
  switch (dur) {
    case DUR_A:
      return codes[DUR_A - 1];
      break;
    case DUR_B:
      return codes[DUR_A + DUR_B - 1];
      break;
    case DUR_C:
      return codes[DUR_A + DUR_B + DUR_C - 1];
      break;
    case DUR_D:
      return codes[DUR_A + DUR_B + DUR_C + DUR_D - 1];
      break;
  }
  return 1000000;
}

bool CCodeDispenser::isUsedCode(u32 code) {
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

void CCodeDispenser::set(u16 vehicle) {
#ifdef SBV
  encService.set(vehicle, mask, max);
  encLoading.set(vehicle + 100, mask, max);
  encAccess.set(vehicle + 200, mask, max);
#endif
#ifdef COIN
  encService.set(vehicle + 300, mask, max);
  encLoading.set(vehicle + 400, mask, max);
  encAccess.set(vehicle + 500, mask, max);
#endif
}

