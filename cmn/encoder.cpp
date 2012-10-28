/*
 * Cencoder8.cpp
 *
 *  Created on: 30 Oct 2009
 *      Author: Wouter
 */

#include "encoder.h"

Cencoder::Cencoder() {
  this->seed = 0;
  this->mask = 0;
  this->max = 999999;
}

Cencoder::Cencoder(u32 _seed , u32 _mask, u32 _max) {
  set(_seed, _mask, _max);
}

void Cencoder::set(u32 _seed , u32 _mask, u32 _max){
  this->seed = _seed;
  this->mask = _mask;
  this->max = _max;
}

u32 Cencoder::getCode(u32 index) {
  u32 lfsr;
  u32 cnt;

  lfsr = seed;
  cnt = 0;
  while (cnt < index) {
    lfsr = (lfsr >> 1) ^ (-(lfsr & 0x1) & mask);
    if (lfsr <= max)
      cnt++;
  }
  return lfsr;
}

void Cencoder::getCodes(u32* codes, sCodeRanges* ranges) {
  u32 lfsr;
  u32 cnt;
  u32 index;
  u08 wrIndex = 0;
  u08 rangeCnt;

  lfsr = seed;
  cnt = 0;
  for (rangeCnt = 0; rangeCnt < ranges->numRanges; rangeCnt++) {
    for (index = ranges->range[rangeCnt].startIndex; index
        < (ranges->range[rangeCnt].startIndex + ranges->range[rangeCnt].numCodes); index++) {
      while (cnt < index) {
        lfsr = (lfsr >> 1) ^ (-(lfsr & 0x1) & mask);
        if (lfsr <= max) {
          cnt++;
        }
      }
      codes[wrIndex] = lfsr;
      wrIndex++;
    }
  }
}

u32 Cencoder::getNextCode(u32 prevCode) {
  prevCode = ((prevCode >> 1) ^ (-(prevCode & 0x1) & mask));
  while (prevCode > max){
    prevCode = ((prevCode >> 1) ^ (-(prevCode & 0x1) & mask));
  }
  return prevCode;
}
