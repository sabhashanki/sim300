#ifndef	SL018_h
#define	SL018_h

#include"types.h"
#include "i2c.h"
#include "pin.h"

using namespace I2C;

void arrayToHex(char *s, u08 array[], u08 len);

#define DATA_LEN  32

typedef struct {
    u08 len;
    u08 cmd;
    u08 data[DATA_LEN];
} sSL018_wr;

typedef struct {
    u08 len;
    u08 cmd;
    u08 sts;
    u08 data[DATA_LEN];
} sSL018_rd;

typedef struct {
    union {
        sSL018_rd rd;
        sSL018_wr wr;
        u08 data[sizeof(sSL018_rd)];
    } uSL018;
} sSL018;

class Csl018 {
  public:
    static const u08 VERSION = 1;
    static const u08 MIFARE_1K = 1;
    static const u08 MIFARE_PRO = 2;
    static const u08 MIFARE_ULTRALIGHT = 3;
    static const u08 MIFARE_4K = 4;
    static const u08 MIFARE_PROX = 5;
    static const u08 MIFARE_DESFIRE = 6;

    static const u08 CMD_IDLE = 0x00;
    static const u08 CMD_SELECT = 0x01;
    static const u08 CMD_LOGIN = 0x02;
    static const u08 CMD_READ16 = 0x03;
    static const u08 CMD_WRITE16 = 0x04;
    static const u08 CMD_READ_VALUE = 0x05;
    static const u08 CMD_WRITE_VALUE = 0x06;
    static const u08 CMD_WRITE_KEY = 0x07;
    static const u08 CMD_INC_VALUE = 0x08;
    static const u08 CMD_DEC_VALUE = 0x09;
    static const u08 CMD_COPY_VALUE = 0x0A;
    static const u08 CMD_READ4 = 0x10;
    static const u08 CMD_WRITE4 = 0x11;
    static const u08 CMD_SET_LED = 0x40;
    static const u08 CMD_VER = 0xF0;
    static const u08 CMD_RESET = 0xFF;

    static const u08 TAG_OK = 0x00;
    static const u08 NO_TAG = 0x01;
    static const u08 LOGIN_OK = 0x02;
    static const u08 LOGIN_FAIL = 0x03;
    static const u08 READ_FAIL = 0x04;
    static const u08 WRITE_FAIL = 0x05;
    static const u08 CANT_VERIFY = 0x06;
    static const u08 COLLISION = 0x0A;
    static const u08 KEY_FAIL = 0x0C;
    static const u08 NO_LOGIN = 0x0D;
    static const u08 NO_VALUE = 0x0E;

    u08 address;
    u08 tagType;
    u08 uid[7];
    u08 uidlen;
    u08 errorCode;
    sSL018 sl018;

    u32 volatile timer;
    Ci2c *i2c;
    Cpin *stpin;
  public:
    Csl018(Ci2c *_i2c, Cpin *_stpin, u08 _addr);
    bool read(void);
    bool present() {
      return (stpin->isEnabled());
    }
    c08* tagName(u08 type);
    bool login(u08 sector);
    bool login(u08 sector, u08 keyType, u08 key[6]);
    c08* getErrorMessage(void);

    bool readBlock(u08 block, u08 *data);
    bool readPage(u08 page, u08 *_data);

    void writeBlock(u08 block, const c08* message); //! Writes a null-terminated string of maximum 15 c08acters to a block
    void writePage(u08 page, const c08* message); //! Writes a null-terminated string of maximum 3 c08acters to a Mifare Ultralight page
    void writeKey(u08 sector, u08 key[6]); //! Write master key (key A)

    void led(bool on);
    bool reset(void);

    c08* getTagName() {
      return tagName(tagType);
    }
    ;
    c08 getErrorCode() {
      return errorCode;
    }
    ;
    u08 getTagType() {
      return tagType;
    }
    ;
  private:
    void clearTime(void);
    u32 getTime(void);
};

#endif
