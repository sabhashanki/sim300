#ifndef IRTC_H
#define IRTC_H

#include "avrlibtypes.h"

namespace IRTC {
  typedef struct {
      u08 sec;
      u08 min;
      u08 hour;
      u08 day;
      u08 mon;
      u08 year;
      u08 wday;
  } sDateTime;

  class Irtc {
    public:
    virtual bool setDate(u16 year, u08 mon, u08 day, u08 hour, u08 min) = 0;
    virtual bool getMinutesFromEpoch(u32* minutes) = 0;
    virtual bool getDate(u16* year, u08* mon, u08* day, u08* hour, u08* min) = 0;

  };
}
#endif
