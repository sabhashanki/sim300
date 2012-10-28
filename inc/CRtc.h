#ifndef CRTC_H
#define CRTC_H

#include "avrlibtypes.h"             // include our global settings
#include "Irtc.h"
#include "CI2c.h"

using namespace I2C;

typedef struct{
u16 year;
u08 mon;
u08 day;
u08 hour;
u08 min;
u08 sec;
}sTimeDate;


class Crtc {
  private:
    u08 rtc_id;
    CI2C* i2c;
  public:
    Crtc(CI2C* _i2c,u08 _id);
    bool setDate(u16 year, u08 mon, u08 day, u08 wday, u08 hour, u08 min);
    bool getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min);
    bool getDate(u16* year, u08* mon, u08* day, u08 *wday, u08* hour, u08* min, u08* sec);
    char* getTimestamp(void);
};

#endif
