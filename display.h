#ifndef CDISPLAY_H_
#define CDISPLAY_H_

#include <avr/pgmspace.h>
#include "types.h"
#include "transport.h"
#include "busprotocol.h"

using namespace LCDKEYPAD;

namespace DISPLAY {
  typedef enum {
    IDLE, CLEAR, SET
  } eState;

  typedef struct {
      u08 update;
      u08 size;
      u08 xPos;
      const char* content;
      prog_char *content_P;
  } sLine;
}
class Cdisplay {
    DISPLAY::eState State;
    Ctransport* Transport;
    sKeypadCmd Cmd;
    sKeypadResp *pRsp;
    bool performClear;
    DISPLAY::sLine Line[4];
    bool sendClear(void);
    bool sendString(u08 StrLen, u08 xPos, u08 yPos, const char* Str);
    bool sendString_P(u08 xPos, u08 yPos, prog_char* Str);
    u08 sendStringHdl;
    u08 sendClearHdl;
    u08 nodeID;
    u08 cnt;
  public:
    Cdisplay(Ctransport* _Transport,u08 _nodeID);
    bool available(void);
    void service(void);
    bool clear(void);
    bool setString(u08 StrLen, u08 xPos, u08 yPos, const char* Str);
    bool setString_P(u08 xPos, u08 yPos, prog_char *buf);
    bool performSet;
};

#endif /* CDISPLAY_H_ */
