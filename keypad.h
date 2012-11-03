#ifndef CKEYPAD_H_
#define CKEYPAD_H_
/****************************************************************************************/
#include "types.h"
#include "transport.h"
#include "busprotocol.h"
/****************************************************************************************/
using namespace LCDKEYPAD;
/****************************************************************************************/
#define ENTER_KEY             10
#define MODE_KEY              11
#define OPEN_KEY              12
#define CLEAR_KEY             13
#define UP_KEY                14
#define DOWN_KEY              15
/****************************************************************************************/
#define KEYBUF_SIZE           32
/****************************************************************************************/
class Ckeypad {
  Ctransport* Transport;
  sKeypadCmd Cmd;
  sKeypadResp *pRsp;
  u08 getKeyHdl;
  u08 getDigitsHdl;
  u08 clearHdl;
  u08 key;
  u08 keys[KEYBUF_SIZE];
  u08 head;
  u08 tail;
  u08 size;
  bool clearPressed;
  bool openPressed;
  bool modePressed;
  bool enterPressed;
  bool upPressed;
  bool downPressed;
  bool getKey(u08* key);
  u08 nodeID;
  u32 atomicTime;
  u16 preval;
  bool trigger;
public:
  volatile u32 time;
  Ckeypad(Ctransport* _Transport, u08 _nodeID);
  void service(void);
  bool getDigit(u08* digit);
  bool getDigits(u32* digits, u08 len);
  void clear(void);
  bool getClear(void);
  bool getEnter(void);
  bool getMode(void);
  bool getOpen(void);
  bool getUp(void);
  bool getDown(void);
};

#endif /* CKEYPAD_H_ */
