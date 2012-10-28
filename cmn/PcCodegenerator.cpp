#ifndef __WIN32__
#include <avr/eeprom.h>
#endif
#include "pccodegenerator.h"

/*---------------------------------------------------------------------------*/
#ifndef __WIN32__
EEMEM u08 perFleet;
EEMEM u08 perFleetIsSet = false;
#endif
/*---------------------------------------------------------------------------*/
cCodeGen::cCodeGen()
{
  #ifndef __WIN32__
  fleetIsSet = eeprom_read_byte(&perFleetIsSet);
  //only read Fleet value if its valid
  if (fleetIsSet==0xcc)
  {
    fleet = eeprom_read_byte(&perFleet);
  }
  else
  {
    fleet = 0;
    fleetIsSet = 0;
  }

  #else
  fleet = 1;
  #endif
}
/*---------------------------------------------------------------------------*/
bool cCodeGen::isValidCode(u32 code, u16 vehicle, eCodeCommand cmd, u32 minutes)
{
  u32           codes[MAX_CODES];
  u08           cntCodes;
  u08           cntDur;
  eCodeDuration dur[NUM_DURATIONS] = {DUR_A, DUR_B, DUR_C, DUR_D};

  for (cntDur = 0 ; cntDur < NUM_DURATIONS ; cntDur++)
  {
    getCodes(codes, vehicle, cmd, minutes, dur[cntDur]);
    for (cntCodes = 0 ; cntCodes < dur[cntDur] ; cntCodes++)
    {
      if (codes[cntCodes] == code) return true;
    }
  }
  return false;
}
/*---------------------------------------------------------------------------*/
void cCodeGen::getCodes(u32* codes, u16 vehicle, eCodeCommand cmd, u32 minutes, eCodeDuration timeDur)
{
  // This is only 19 bits of the 20 in the combined LFSR
  u32 cnt;
  u32 index;

  cmdLFSR = cmd;
  vehLFSR = vehicle;
  fleetLFSR = fleet;

  index = (minutes % 999999);
  switch (timeDur) {
        case DUR_B:
                index = ((index + DUR_SEPERATION) % 999999);
                break;
        case DUR_C:
                index = ((index + (2*DUR_SEPERATION)) % 999999);
                break;
        case DUR_D:
                index = ((index + (3*DUR_SEPERATION)) % 999999);
                break;
  }
  for (cnt = 0; cnt < (index-timeDur) ; cnt++)
  {
    getNextCode();
  }
  for (cnt = 0; cnt < (u08)timeDur; cnt++)
  {
    codes[cnt] = getNextCode();
  }
}
/*---------------------------------------------------------------------------*/
u32 cCodeGen::getNextCode(void)
{
  bool codeFound = false;
  u32  bit;
  u32  code;

  while(!codeFound)
  {
    bit = (cmdLFSR & (0x0001 << 1) >> 1) ^ ((cmdLFSR & (0x0001 << 2)) >> 2);
    cmdLFSR = (cmdLFSR >> 1) | (bit << 2);
    cmdLFSR &= 0x07;
    bit = (vehLFSR & (0x0001 << 6) >> 6) ^ ((vehLFSR & (0x0001 << 9)) >> 9);
    vehLFSR = (vehLFSR >> 1) | (bit << 9);
    vehLFSR &= 0x3FF;
    bit = (fleetLFSR & (0x0001 << 4) >> 4) ^ ((fleetLFSR & (0x0001 << 5)) >> 5);
    fleetLFSR = (fleetLFSR >> 1) | (bit << 5);
    fleetLFSR &= 0x3F;
    code = ((fleetLFSR) << 13) | ((vehLFSR) << 3)  | (cmdLFSR);
    if (code < 1000000)
    {
    	codeFound = true;
    }
  }
  return code;
}
/*---------------------------------------------------------------------------*/
bool cCodeGen::setFleet(u08 _fleet)
{
  if (fleetIsSet!=0xcc)
  {
    fleet = _fleet;
    fleetIsSet = 0xcc;
    #ifndef __WIN32__
    eeprom_write_byte(&perFleetIsSet, (u08)0xcc);
    eeprom_write_byte(&perFleet, (u08)fleet);
    #endif
    return true;
  }
  return false;
}


bool cCodeGen::getFleetStatus(void)
{
	if (fleetIsSet==0xcc)
	{
		return true;
	}
	else
	{
		return false;
	}
}

u08 cCodeGen::getFleet(void)
{
  return fleet;
}
/*---------------------------------------------------------------------------*/
bool cCodeGen::getCode(u32* code, u16 vehicle, eCodeCommand cmd, u32 minutes, eCodeDuration timeDur)
{
  u32 codes[MAX_CODES];

  getCodes(codes, vehicle, cmd, minutes, timeDur);
  *code = codes[timeDur-1];
  return true;
}
/*---------------------------------------------------------------------------*/
