/****************************************************************************************/
// The Tfifo template class implements a type generic First-In-First-Out buffer.
//
//   Created : 15 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef TFIFO_H_
#define TFIFO_H_
/****************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>
/****************************************************************************************/
#include "types.h"
/****************************************************************************************/
template<class T>
class Tfifo {
  private:
    u16 headIndex;
    u16 tailIndex;
    u16 size;
    u16 _used;
    void* buffer;
  public:
    Tfifo(void);
    bool setBufSize(u16 numBlocks);
    u16 write(T* src, u16 numBlocks);
    u16 write(Tfifo<T>* src);
    u16 read(T* dst, u16 numBlocks, bool keep);
    u16 read(Tfifo<T>* _dst, u16 numBlocks, bool keep);
    bool readToStr(c08* dst);
    u16 space(void);
    u16 used(void) {
      return _used;
    }
    bool empty(void);
    void clear(void);
};
/****************************************************************************************/
template<class T>
Tfifo<T>::Tfifo(void) {
  size = 0;
  headIndex = 0;
  tailIndex = 0;
  buffer = 0;
  _used = 0;
}
/****************************************************************************************/
template<class T>
bool Tfifo<T>::setBufSize(u16 numBlocks) {
  do {
    buffer = malloc(numBlocks * sizeof(T));
  } while (buffer == NULL);
  size = numBlocks;
  return true;
}
/****************************************************************************************/
template<class T>
u16 Tfifo<T>::space(void) {
  return (size - _used);
}
/****************************************************************************************/
template<class T>
u16 Tfifo<T>::write(T* _src, u16 numBlocks) {
  void* dst;
  void* src = _src;
  u16 cnt = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    while (cnt < numBlocks && _used < size) {
      dst = (void*) ((u16) buffer + (headIndex * sizeof(T)));
      memcpy(dst, src, sizeof(T));
      src = (void*) ((u16) src + sizeof(T));
      headIndex = (headIndex + 1) % size;
      cnt++;
      _used++;
    }
  }
  return cnt;
}
/****************************************************************************************/
template<class T>
u16 Tfifo<T>::write(Tfifo<T>* src) {
  void* dst;
  T _src;
  u16 cnt = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    while (!src->empty() && _used < size) {
      dst = (void*) ((u16) buffer + (headIndex * sizeof(T)));
      src->read(&_src, 1);
      memcpy(dst, &_src, sizeof(T));
      headIndex = (headIndex + 1) % size;
      cnt++;
      _used++;
    }
  }
  return cnt;
}
/****************************************************************************************/
template<class T>
u16 Tfifo<T>::read(T* _dst, u16 numBlocks = 0, bool keep = false) {
  void* dst = _dst;
  void* src;
  u08 keepTail = tailIndex;
  u08 keepUsed = _used;
  u16 cnt = 0;
  if (numBlocks == 0)
    numBlocks = _used;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    while (cnt < numBlocks && _used > 0) {
      src = (void*) ((u16) buffer + (tailIndex * sizeof(T)));
      memcpy(dst, src, sizeof(T));
      dst = (void*) ((u16) dst + sizeof(T));
      tailIndex = (tailIndex + 1) % size;
      cnt++;
      _used--;
    }
    if (keep) {
      tailIndex = keepTail;
      _used = keepUsed;
    }
  }
  return cnt;
}

/****************************************************************************************/
template<class T>
u16 Tfifo<T>::read(Tfifo<T>* _dst, u16 numBlocks = 0, bool _keep = false) {
  T dst;
  void* src;
  u08 keepTail = tailIndex;
  u08 keepUsed = _used;
  u16 cnt = 0;
  if (numBlocks == 0)
    numBlocks = _used;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    while (cnt < numBlocks && _used > 0) {
      src = (void*) ((u16) buffer + (tailIndex * sizeof(T)));
      memcpy(&dst, src, sizeof(T));
      _dst->write(&dst, 1);
      tailIndex = (tailIndex + 1) % size;
      cnt++;
      _used--;
    }
    if (_keep) {
      tailIndex = keepTail;
      _used = keepUsed;
    }
  }
  return cnt;
}

/****************************************************************************************/
template<class T>
bool Tfifo<T>::readToStr(c08* str) {
  u08 idx = _used;
  if (_used == 0)
    return false;
  read((u08*) str, _used);
  str[idx] = 0;
  return true;
}
/****************************************************************************************/
template<class T>
bool Tfifo<T>::empty(void) {
  return (_used == 0);
}
/****************************************************************************************/
template<class T>
void Tfifo<T>::clear(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    tailIndex = 0;
    headIndex = 0;
    _used = 0;
  }
}
/****************************************************************************************/
#endif
