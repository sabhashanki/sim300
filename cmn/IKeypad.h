#ifndef IKEYPAD_H
#define IKEYPAD_H
/********************************* KEYPAD LCD Opcodes ***********************************/
namespace IKEYPAD {
  class IKeypad {
    public:
      virtual bool clear(void) = 0;
      virtual bool getKey(u08 *key) = 0;
      virtual bool getDecimal(u08 digits, u32* num) = 0;
  };
}
#endif
