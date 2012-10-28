#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
/*---------------------------------------------------------------------------*/
#include "avrlibtypes.h"
#ifndef __WIN32__
        #include "common.h"
        #include "iopins.h"
#endif        
/*---------------------------------------------------------------------------*/
#define NUM_DURATIONS   4
#define MAX_CODES       16    // Must be the same as the highest number in eCodeDuration
#define DUR_SEPERATION  10000 // Seperation distance between duration zones.
/*---------------------------------------------------------------------------*/

//* use must hook up a led here*/
//#define TOGGLE_LED()    LED7_TOGGLE()

typedef enum
{
  DUR_A = 2,
  DUR_B = 4,
  DUR_C = 8,
  DUR_D = 16
} eCodeDuration;
/*---------------------------------------------------------------------------*/
typedef enum
{
  CODE_CMD_SERVICE = 1,
  CODE_CMD_LOADING = 2,
  CODE_CMD_OPERATIONAL = 3,
  CODE_CMD_LOCKDOWN = 4
} eCodeCommand;
/*---------------------------------------------------------------------------*/
class cCodeGen
{
  // Commands from 1 to 7
  // Reg width = 3
  u32 cmdLFSR;
  // Vehicle numbers from 1 to 1023
  // Reg width = 10
  u32 vehLFSR;
  // Fleet numbers from 1 to 63
  // Reg width = 6
  u32 fleetLFSR;
  u08 fleet;
  u08 fleetIsSet;
  u32 getNextCode(void);
  public:
  cCodeGen(void);
  bool isValidCode(u32 code, u16 vehicle, eCodeCommand cmd, u32 minutes);
  void getCodes(u32* codes, u16 vehicle, eCodeCommand cmd, u32 minutes, eCodeDuration timeDur);
  bool getCode(u32* code, u16 vehicle, eCodeCommand cmd, u32 minutes, eCodeDuration timeDur);
  bool setFleet(u08 fleet);
  u08 getFleet(void);
  bool getFleetStatus(void);

};
/*---------------------------------------------------------------------------*/
#endif
