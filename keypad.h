#ifndef CKEYPAD_H_
#define CKEYPAD_H_
/****************************************************************************************/
#include "busprotocol.h"
#include "transport.h"
#include "types.h"
#include "scheduler.h"
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
class Ckeypad: public Csignal {
    static const u08 DEFAULT_KEYPAD_NODE_ID = 0xFC;
    static const f32 period = 1;
    bool getKey(u08* key);
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
    u08 nodeID;
    u16 preval;
  public:
    Ckeypad(Ctransport* _transport, u08 _nodeID = DEFAULT_KEYPAD_NODE_ID);
    bool readKey(u08* key);
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
