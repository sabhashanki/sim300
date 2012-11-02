#ifndef __WIN32__
#include <avr/eeprom.h>
#include <util/atomic.h>
#endif
#include "codegenerator.h"
#include "crc.h"
/*---------------------------------------------------------------------------*/
#define MAX_CODE 1000000
#define INVALID_CODE_TABLE  0xA5
#define VALID_CODE_TABLE    0x5A
/*---------------------------------------------------------------------------*/
cCodeGen::cCodeGen() {
  currentCodeTime = 0;
  currentTime = 0;
  index = 0;
  head = 0;
  codesValid = false;
  codesStored = false;
}
/*---------------------------------------------------------------------------*/
void cCodeGen::set(eCodeCommand _cmd, eCodeDuration _duration, u16 _vehicle, u08 _fleet,
                   u08* _storeAdr) {
  storeAdr = _storeAdr;
  retrieve();
  if (!retrieved || cmd != _cmd || duration != _duration || vehicle != _vehicle || fleet
      != _fleet) {
    duration = _duration;
    cmd = _cmd;
    vehicle = _vehicle;
    fleet = _fleet;
    cmdLFSR = cmd;
    vehLFSR = vehicle;
    fleetLFSR = fleet;
    switch (duration) {
      case DUR_A:
        offset = 0;
        break;
      case DUR_B:
        offset = DUR_SEPERATION;
        break;
      case DUR_C:
        offset = DUR_SEPERATION + DUR_SEPERATION;
        break;
      case DUR_D:
        offset = DUR_SEPERATION + DUR_SEPERATION + DUR_SEPERATION;
        break;
    }
    indexed = false;
  }
}
/*---------------------------------------------------------------------------*/
u32 cCodeGen::getCode(void) {
  if (head == 0)
    return codes[duration - 1];
  else
    return codes[head - 1];
}
/*---------------------------------------------------------------------------*/
void cCodeGen::service() {
  u32 distance;
  if (indexed) {
    while (currentCodeTime < (currentTime % 999999)) {
      distance = ((currentTime % 999999) - currentCodeTime);
      if (distance > DUR_MAX) {
        distance = distance - DUR_MAX;
        advanceCodes(distance);
        currentCodeTime = (currentCodeTime + distance) % 999999;
        codesStored = false;
      }
      codes[head] = getNextCode();
      head = (head + 1) % duration;
      currentCodeTime = (currentCodeTime + 1) % 999999;
      codesValid = false;
    }
    if (currentCodeTime == (currentTime % 999999)) {
      codesValid = true;
      if (!codesStored) {
        store();
      }
    }
  } else {
    advanceCodes(offset);
    indexed = true;
  }
}
/*---------------------------------------------------------------------------*/
void cCodeGen::setCurrentTime(u32 newTime) {
  if (newTime < currentTime) {
    currentCodeTime = 0;
    index = 0;
    head = 0;
    indexed = false;
    cmdLFSR = cmd;
    vehLFSR = vehicle;
    fleetLFSR = fleet;
  }
  currentTime = newTime;
  codesValid = false;
}
/*---------------------------------------------------------------------------*/
bool cCodeGen::isValidCode(u32 code) {
  u08 cntCodes;

  while (!codesValid) {
    service();
  }
  for (cntCodes = 0; cntCodes < duration; cntCodes++) {
    if (codes[cntCodes] == code) {
      codes[cntCodes] = MAX_CODE;
      store();
      return true;
    }
  }
  return false;
}
/*---------------------------------------------------------------------------*/
u32 cCodeGen::getNextCode(void) {
  bool codeFound = false;
  u32 bit;
  u32 code;

  while (!codeFound) {
    bit = (cmdLFSR & (0x0001 << 1) >> 1) ^ ((cmdLFSR & (0x0001 << 2)) >> 2);
    cmdLFSR = (cmdLFSR >> 1) | (bit << 2);
    cmdLFSR &= 0x07;
    bit = (vehLFSR & (0x0001 << 6) >> 6) ^ ((vehLFSR & (0x0001 << 9)) >> 9);
    vehLFSR = (vehLFSR >> 1) | (bit << 9);
    vehLFSR &= 0x3FF;
    bit = (fleetLFSR & (0x0001 << 4) >> 4) ^ ((fleetLFSR & (0x0001 << 5)) >> 5);
    fleetLFSR = (fleetLFSR >> 1) | (bit << 5);
    fleetLFSR &= 0x3F;
    code = ((fleetLFSR) << 13) | ((vehLFSR) << 3) | (cmdLFSR);
    if (code < 1000000) {
      codeFound = true;
    }
  }
  return code;
}
/*---------------------------------------------------------------------------*/
void cCodeGen::advanceCodes(u32 steps) {
  bool codeFound = false;
  u32 bit;
  u32 code;
  u32 tmpCmdLFSR;
  u32 tmpVehLFSR;
  u32 tmpFleetLFSR;
  u32 cnt;

  tmpCmdLFSR = cmdLFSR;
  tmpVehLFSR = vehLFSR;
  tmpFleetLFSR = fleetLFSR;
  for (cnt = 0; cnt < steps; cnt++) {
    codeFound = false;
    while (!codeFound) {
      bit = (tmpCmdLFSR & (0x0001 << 1) >> 1) ^ ((tmpCmdLFSR & (0x0001 << 2)) >> 2);
      tmpCmdLFSR = (tmpCmdLFSR >> 1) | (bit << 2);
      tmpCmdLFSR &= 0x07;
      bit = (tmpVehLFSR & (0x0001 << 6) >> 6) ^ ((tmpVehLFSR & (0x0001 << 9)) >> 9);
      tmpVehLFSR = (tmpVehLFSR >> 1) | (bit << 9);
      tmpVehLFSR &= 0x3FF;
      bit = (tmpFleetLFSR & (0x0001 << 4) >> 4) ^ ((tmpFleetLFSR & (0x0001 << 5)) >> 5);
      tmpFleetLFSR = (tmpFleetLFSR >> 1) | (bit << 5);
      tmpFleetLFSR &= 0x3F;
      code = ((tmpFleetLFSR) << 13) | ((tmpVehLFSR) << 3) | (tmpCmdLFSR);
      if (code < MAX_CODE) {
        codeFound = true;
      }
    }
  }
  cmdLFSR = tmpCmdLFSR;
  vehLFSR = tmpVehLFSR;
  fleetLFSR = tmpFleetLFSR;
}
/*---------------------------------------------------------------------------*/
void cCodeGen::store(void) {
  sSafeStore tmp;
  u08* src;
  u08* dst;
  u08 cnt;
  tmp.data.cmdLFSR = cmdLFSR;
  tmp.data.vehLFSR = vehLFSR;
  tmp.data.fleetLFSR = fleetLFSR;
  tmp.data.vehicle = vehicle;
  tmp.data.fleet = fleet;
  tmp.data.cmd = cmd;
  tmp.data.duration = duration;
  tmp.data.currentCodeTime = currentCodeTime;
  tmp.data.currentTime = currentTime;
  tmp.data.offset = offset;
  tmp.data.head = head;
  tmp.data.indexed = indexed;
  tmp.data.index = index;
  for (cnt = 0; cnt < DUR_MAX; cnt++) {
    tmp.data.codes[cnt] = codes[cnt];
  }
  // Invalidate the block
  dst = storeAdr + (&tmp.valid - (u08*) &tmp.data);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_write_byte(dst, INVALID_CODE_TABLE);
  }
  tmp.valid = VALID_CODE_TABLE;
  // Calculate the CRC
  tmp.crc = crc8((u08*) &tmp.data, sizeof(sStore));
  src = (u08*) (&tmp.data);
  dst = (u08*) (storeAdr);
  for (cnt = 0; cnt < (sizeof(sSafeStore)); cnt++) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      eeprom_write_byte(dst, *src);
      dst++;
      src++;
    }
  }
  codesStored = true;
}
/*---------------------------------------------------------------------------*/
void cCodeGen::retrieve(void) {
  sSafeStore tmp;
  u08 cnt;
  u08 crc;
  retrieved = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    eeprom_read_block(&tmp, (void*) storeAdr, sizeof(sSafeStore));
  }
  crc = crc8((u08*) &tmp.data, sizeof(tmp.data));
  if (tmp.valid == VALID_CODE_TABLE && crc == tmp.crc) {
    cmdLFSR = tmp.data.cmdLFSR;
    vehLFSR = tmp.data.vehLFSR;
    fleetLFSR = tmp.data.fleetLFSR;
    vehicle = tmp.data.vehicle;
    fleet = tmp.data.fleet;
    cmd = tmp.data.cmd;
    duration = tmp.data.duration;
    currentCodeTime = tmp.data.currentCodeTime;
    currentTime = tmp.data.currentTime;
    offset = tmp.data.offset;
    head = tmp.data.head;
    indexed = tmp.data.indexed;
    index = tmp.data.index;
    for (cnt = 0; cnt < DUR_MAX; cnt++) {
      codes[cnt] = tmp.data.codes[cnt];
    }
    retrieved = true;
  }
}
/*---------------------------------------------------------------------------*/
