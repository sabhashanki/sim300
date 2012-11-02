#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
/*---------------------------------------------------------------------------*/
#include "avrlibtypes.h"
#include "common.h"
/*---------------------------------------------------------------------------*/
#define NUM_DURATIONS   4
#define NUM_COMMANDS    4
#define DUR_SEPERATION  10000 // Separation distance between duration zones.
/*---------------------------------------------------------------------------*/

//* use must hook up a led here*/
#define TOGGLE_LED()    LED7_TOGGLE()

typedef enum {
  DUR_A = 2, DUR_B = 4, DUR_C = 8, DUR_D = 16, DUR_MAX = DUR_D
} eCodeDuration;
/*---------------------------------------------------------------------------*/

typedef enum {
  CODE_CMD_SERVICE = 1,
    CODE_CMD_LOADING = 2,
    CODE_CMD_OPERATIONAL = 3,
    CODE_CMD_LOCKDOWN = 4
} eCodeCommand;

typedef struct {
    u32 cmdLFSR;
    u32 vehLFSR;
    u32 fleetLFSR;
    u16 vehicle;
    u08 fleet;
    eCodeCommand cmd;
    eCodeDuration duration;
    u32 currentCodeTime;
    u32 currentTime;
    u32 offset;
    u08 head;
    u32 codes[DUR_MAX];
    u08 indexed;
    u08 codesStored;
    u32 index;
} sStore;
/*---------------------------------------------------------------------------*/
typedef struct {
    sStore data;
    u08 crc;
    u08 valid;
} sSafeStore;
/*---------------------------------------------------------------------------*/
class cCodeGen {
  private:
    // Commands from 1 to 7
    // Reg width = 3
    u32 cmdLFSR;
    // Vehicle numbers from 1 to 1023
    // Reg width = 10
    u32 vehLFSR;
    // Fleet numbers from 1 to 63
    // Reg width = 6
    u32 fleetLFSR;
    u16 vehicle;
    u08 fleet;
    eCodeCommand cmd;
    eCodeDuration duration;
    u32 currentCodeTime;
    u32 currentTime;
    u32 offset;
    u08 head;
    u32 codes[DUR_MAX];
    u08 indexed;
    u08* storeAdr;
    u08 codesStored;
    u32 index;
    bool retrieved;
    u32 getNextCode(void);
    void advanceCodes(u32 steps);
  public:
    u08 codesValid;
    cCodeGen(void);
    void set(eCodeCommand cmd, eCodeDuration duration, u16 vehicle, u08 fleet,
             u08* _storeAdr);
    u32 getCode(void);
    void service(void);
    void setCurrentTime(u32 newTime);
    bool isValidCode(u32 code);
    void store(void);
    void retrieve(void);
};
/*---------------------------------------------------------------------------*/
#endif
