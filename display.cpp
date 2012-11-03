/*
 * CDisplay.cpp
 *
 *  Created on: 08 Mar 2009
 *      Author: Wouter
 */

#include <string.h>

#include "display.h"

using namespace DISPLAY;
using namespace LCDKEYPAD;

Cdisplay::Cdisplay(Ctransport* _Transport, u08 _nodeID) {
  State = IDLE;
  performClear = false;
  performSet = false;
  Transport = _Transport;
  for (cnt = 0; cnt < 4; cnt++) {
    Line[cnt].update = 0;
    Line[cnt].size = 0;
    Line[cnt].xPos = 0;
  }
  sendStringHdl = handleEmpty;
  sendClearHdl = handleEmpty;
  nodeID = _nodeID;
  cnt = 0;
}

bool Cdisplay::available(void) {
  return (State == IDLE && performSet == false && performClear == false);
}

void Cdisplay::service(void) {

  switch (State) {
    case IDLE:
      if (performClear) {
        State = CLEAR;
      } else if (performSet) {
        cnt = 0;
        State = SET;
      }
      break;
    case CLEAR:
      if (sendClear()) {
        State = IDLE;
        performClear = false;
      }
      break;
    case SET:
      if (cnt < 4) {
        if (Line[cnt].update) {
          if (Line[cnt].update == 1) {
            if (sendString(Line[cnt].size, Line[cnt].xPos, cnt, Line[cnt].content)) {
              Line[cnt].update = false;
            }
          } else if (Line[cnt].update == 2) {
            if (sendString_P(Line[cnt].xPos, cnt, Line[cnt].content_P)) {
              Line[cnt].update = false;
            }
          }
        } else {
          cnt++;
        }
      } else {
        performSet = false;
        State = IDLE;
      }
      break;
  }
}

bool Cdisplay::clear(void) {
  performClear = true;
  return true;
}

bool Cdisplay::setString(u08 StrLen, u08 xPos, u08 yPos, const char* Str) {
  if (!Line[yPos].update) {
    Line[yPos].size = StrLen;
    Line[yPos].xPos = xPos;
    Line[yPos].content = Str;
    Line[yPos].update = 1;
    performSet = true;
    return true;
  }
  return false;
}

bool Cdisplay::setString_P(u08 xPos, u08 yPos, prog_char *buf) {
  if (!Line[yPos].update) {
    Line[yPos].size = strlen_P(buf);
    Line[yPos].xPos = xPos;
    Line[yPos].content_P = buf;
    Line[yPos].update = 2;
    performSet = true;
    return true;
  }
  return false;
}

bool Cdisplay::writeClear(void) {
  sKeypadResp rsp;
  memset(&Cmd, 0, sizeof(sKeypadCmd));
  Cmd.Opcode = CLEAR_LCD;
  if (!Transport->read((u08*) &rsp, (u08*) &Cmd, sizeof(sKeypadCmd), nodeID)) {
    return false;
  }
  if (rsp.Hdr.Opcode != Cmd.Opcode || !rsp.Hdr.Result) {
    return false;
  }
  return true;
}

bool Cdisplay::sendClear(void) {
  memset(&Cmd, 0, sizeof(sKeypadCmd));
  Cmd.Opcode = CLEAR_LCD;
  sendClearHdl = Transport->tx(sendClearHdl, nodeID, (u08*) &Cmd, sizeof(sKeypadCmd),
                               (u08**) &pRsp);
  if (sendClearHdl == handleDone) {
    sendClearHdl = handleEmpty;
    if (pRsp->Hdr.Opcode == CLEAR_LCD) {
      if (pRsp->Hdr.Result == true) {
        return true;
      }
    }
  }
  return false;
}

bool Cdisplay::writeString(u08 StrLen, u08 xPos, u08 yPos, const char* Str) {
  sKeypadResp rsp;
  memset(&Cmd, 0, sizeof(sKeypadCmd));
  Cmd.Opcode = SET_LCD_STRING;
  Cmd.Dat.DisplayText.StrLen = StrLen;
  Cmd.Dat.DisplayText.xPos = xPos;
  Cmd.Dat.DisplayText.yPos = yPos;
  memcpy(Cmd.Dat.DisplayText.Str, Str, StrLen);
  if (!Transport->read((u08*) &rsp, (u08*) &Cmd, sizeof(sKeypadCmd), nodeID)) {
    return false;
  }
  if (rsp.Hdr.Opcode != Cmd.Opcode || !rsp.Hdr.Result) {
    return false;
  }
  return true;
}


bool Cdisplay::sendString(u08 StrLen, u08 xPos, u08 yPos, const char* Str) {
  u08 cnt;
  memset(&Cmd, 0, sizeof(sKeypadCmd));
  Cmd.Opcode = SET_LCD_STRING;
  Cmd.Dat.DisplayText.StrLen = StrLen;
  Cmd.Dat.DisplayText.xPos = xPos;
  Cmd.Dat.DisplayText.yPos = yPos;
  for (cnt = 0; cnt < StrLen; cnt++) {
    Cmd.Dat.DisplayText.Str[cnt] = *Str;
    Str++;
  }
  sendStringHdl = Transport->tx(sendStringHdl, nodeID, (u08*) &Cmd, sizeof(sKeypadCmd),
                                (u08**) &pRsp);
  if (sendStringHdl == handleDone) {
    sendStringHdl = handleEmpty;
    if (pRsp->Hdr.Opcode == SET_LCD_STRING) {
      if (pRsp->Hdr.Result == true) {
        return true;
      }
    }
  }
  return false;
}

bool Cdisplay::sendString_P(u08 xPos, u08 yPos, prog_char* Str) {
  memset(&Cmd, 0, sizeof(sKeypadCmd));
  Cmd.Opcode = SET_LCD_STRING;
  Cmd.Dat.DisplayText.StrLen = strlen_P(Str);
  Cmd.Dat.DisplayText.xPos = xPos;
  Cmd.Dat.DisplayText.yPos = yPos;
  strcpy_P((c08*) Cmd.Dat.DisplayText.Str, Str);
  sendStringHdl = Transport->tx(sendStringHdl, nodeID, (u08*) &Cmd, sizeof(sKeypadCmd),
                                (u08**) &pRsp);
  if (sendStringHdl == handleDone) {
    sendStringHdl = handleEmpty;
    if (pRsp->Hdr.Opcode == SET_LCD_STRING) {
      if (pRsp->Hdr.Result == true) {
        return true;
      }
    }
  }
  return false;
}

