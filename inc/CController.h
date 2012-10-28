#ifndef CCONTROLLER_H
#define CCONTROLLER_H

#include <avr/eeprom.h>

#include "CServer.h"
#include "main.h"
#include "CRfid.h"
#include "CModem.h"

#define NUM_OF_SIGNALS  2
#define TAGTABLE_SIGNAL 0
#define MODEM_SIGNAL    1


#define MAX_SMS_PHONENUMBERS  2



typedef enum{
  ACCESS_DISABLED,
  ACCESS_AUTHORISED,
  ACCESS_GRANTED
}eSiteState;


namespace CCONTROLLER {
  class CController {
    private:
      u08 smsCnt;
      u08 Authorise;
      CModem *modem;
      CServer *server;
      u08 SiteID;
      u08 idValid;
      u08 ipValid;
      CRFID* Rfid;
      sSignal signal[NUM_OF_SIGNALS];
      c08 PhoneNumbers[MAX_SMS_PHONENUMBERS][12];
      c08 sim1PIN[6];
      c08 sim2PIN[6];
      u08 nrValid;
      u08 PIN1Invalid;
      void idle(void);
      void consoleService(void);
      void signalsService(void);
      void TagTableService(void);
    public:
      volatile u32 timer;
      CController(CRFID* _Rfid,CModem *_modem,CServer *_server);
      void Service(void);
      void HandleRxData(void);
      void Setup(void);
  };
}
#endif


