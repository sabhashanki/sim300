/*
 * CKeypad.cpp
 *
 *  Created on: 08 Mar 2009
 *      Author: Wouter
 */

#include <string.h>
#include <util/atomic.h>
#include "keypad.h"

Ckeypad::Ckeypad(Ctransport* _transport, u08 _nodeID) :
    Csignal(period) {
  Transport = _transport;
  getKeyHdl = handleEmpty;
  getDigitsHdl = handleEmpty;
  clearHdl = handleEmpty;
  head = 0;
  tail = 0;
  size = 0;
  clearPressed = false;
  openPressed = false;
  modePressed = false;
  enterPressed = false;
  upPressed = false;
  downPressed = false;
  nodeID = _nodeID;
  scheduler.attach(this);
}

void Ckeypad::service() {
  if (isSet()) {
    if (readKey(&key)) {
      switch (key) {
        case ENTER_KEY:
          enterPressed = true;
          break;
        case MODE_KEY:
          modePressed = true;
          break;
        case OPEN_KEY:
          openPressed = true;
          break;
        case CLEAR_KEY:
          clearPressed = true;
          break;
        case UP_KEY:
          upPressed = true;
          break;
        case DOWN_KEY:
          downPressed = true;
          break;
        default:
          if (size < (KEYBUF_SIZE - 1)) {
            keys[head] = key;
            head = (head + 1) % KEYBUF_SIZE;
            size++;
          }
          break;
      }
    }
  }
}

bool Ckeypad::readKey(u08* key) {
  sKeypadResp rsp;
  Cmd.Opcode = GET_KEYPAD_KEY;
  if (!Transport->read((u08*)&rsp, (u08*) &Cmd, sizeof(sKeypadCmd), nodeID)) {
    return false;
  }
  if (rsp.Hdr.Opcode != Cmd.Opcode || !rsp.Hdr.Result) {
    return false;
  }
  *key = rsp.Dat.Key;
  return true;
}

bool Ckeypad::getKey(u08* key) {
  Cmd.Opcode = GET_KEYPAD_KEY;
  getKeyHdl = Transport->tx(getKeyHdl, nodeID, (u08*) &Cmd, sizeof(sKeypadCmd),
                            (u08**) &pRsp);
  if (getKeyHdl == handleDone) {
    getKeyHdl = handleEmpty;
    if (pRsp->Hdr.Opcode == GET_KEYPAD_KEY) {
      if (pRsp->Hdr.Result == true) {
        *key = pRsp->Dat.Key;
        return true;
      }
    }
  }
  return false;
}

bool Ckeypad::getClear() {
  if (clearPressed) {
    clearPressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getEnter(void) {
  if (enterPressed) {
    enterPressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getMode(void) {
  if (modePressed) {
    modePressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getOpen(void) {
  if (openPressed) {
    openPressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getUp(void) {
  if (upPressed) {
    upPressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getDown(void) {
  if (downPressed) {
    downPressed = false;
    return true;
  }
  return false;
}

bool Ckeypad::getDigit(u08* digit) {
  if (size > 0) {
    *digit = keys[tail];
    tail = (tail + 1) % KEYBUF_SIZE;
    size--;
    return true;
  }
  return false;
}

bool Ckeypad::getDigits(u32* digits, u08 len) {
  u08 cnt;
  u32 decimal = 1;
  *digits = 0;
  for (cnt = 1; cnt < len; cnt++) {
    decimal = decimal * 10;
  }
  if (getEnter()) {
    if (size >= len) {
      while (size > len) {
        tail = (tail + 1) % KEYBUF_SIZE;
        size--;
      }
      for (cnt = 0; cnt < len; cnt++) {
        *digits += (keys[tail] * decimal);
        tail = (tail + 1) % KEYBUF_SIZE;
        size--;
        decimal = decimal / 10;
      }
      clear();
      return true;
    }
    clear();
  }
  return false;
}

void Ckeypad::clear(void) {
  head = 0;
  tail = 0;
  size = 0;
  clearPressed = false;
  openPressed = false;
  modePressed = false;
  enterPressed = false;
  upPressed = false;
  downPressed = false;
}

