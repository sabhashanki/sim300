#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "sl018.h"
using namespace I2C;
/*****************************************************************************************/
Csl018::Csl018(Ci2c *_i2c, Cpin *_stpin, u08 _addr) {
  i2c = _i2c;
  address = _addr;
  stpin = _stpin;
  scheduler.attach(&timeout);
}
/*****************************************************************************************/
bool Csl018::reset(void) {
  sl018.uSL018.wr.cmd = CMD_RESET;
  sl018.uSL018.wr.len = 1;
  i2c->masterSend(address, sl018.uSL018.wr.len, sl018.uSL018.data);
  timeout.wait(500e-3);
  return true;
}
/*****************************************************************************************/
bool Csl018::read(void) {
  u08 size;
  sl018.uSL018.wr.cmd = CMD_SELECT;
  sl018.uSL018.wr.len = 2;
  i2c->masterSend(address, sl018.uSL018.wr.len, sl018.uSL018.data);
  timeout.wait(5e-3);
  sl018.uSL018.rd.len = 11;
  i2c->masterReceive(address, sl018.uSL018.rd.len, sl018.uSL018.data);
  errorCode = sl018.uSL018.rd.sts;
  if (errorCode == TAG_OK && sl018.uSL018.rd.cmd == CMD_SELECT && sl018.uSL018.rd.len > 0
      && sl018.uSL018.rd.len < 16) {
    size = (sl018.uSL018.rd.len - 3); //Subtract the len cmd and status bytes
    memcpy(uid, &sl018.uSL018.rd.data, size);
    uidlen = size;
    tagType = sl018.uSL018.rd.data[size];
    return true;
  }
  return false;
}
/*****************************************************************************************/
c08* Csl018::getErrorMessage(void) {
  switch (errorCode) {
    case 0:
      return "OK";
    case 1:
      return "No tag present";
    case 2:
      return "Login OK";
    case 3:
    case 0x10:
      return "Login failed";
    case 4:
      return "Read failed";
    case 5:
      return "Write failed";
    case 6:
      return "Unable to read after write";
    case 0x0A:
      return "Collision detected";
    case 0x0C:
      return "Load key failed";
    case 0x0D:
      return "Not authenticated";
    case 0x0E:
      return "Not a value block";
    default:
      return "Unknown error";
  }
}

/*****************************************************************************************/
/** Authenticate with transport key (0xFFFFFFFFFFFF).
 *	@param sector Sector number
 */
bool Csl018::login(u08 sector) {
  sl018.uSL018.wr.cmd = CMD_LOGIN;
  sl018.uSL018.wr.len = 9;
  sl018.uSL018.wr.data[0] = sector;
  sl018.uSL018.wr.data[1] = 0xAA;
  sl018.uSL018.wr.data[2] = 0xAA;
  memset(&sl018.uSL018.wr.data[3], 0xFF, 6);
  i2c->masterSend(address, sl018.uSL018.wr.len, sl018.uSL018.data);
  sl018.uSL018.rd.len = 3;
  timeout.wait(10e-3);
  i2c->masterReceive(address, sl018.uSL018.rd.len, sl018.uSL018.rd.data);
  errorCode = sl018.uSL018.rd.sts;
  if (errorCode == LOGIN_OK && sl018.uSL018.rd.cmd == CMD_LOGIN
      && (sl018.uSL018.rd.len > 0)) {
    return true;
  }
  return false;
}

/*****************************************************************************************/
/** Log in with specified key A or key B.
 *	@param sector Sector number
 *	@param keyType Which key to use: 0xAA for key A or 0xBB for key B
 *	@param key Key value (6 bytes)
 */
bool Csl018::login(u08 sector, u08 keyType, u08 key[6]) {
  sl018.uSL018.wr.cmd = CMD_LOGIN;
  sl018.uSL018.wr.len = 9;
  sl018.uSL018.wr.data[0] = sector;
  sl018.uSL018.wr.data[1] = keyType;
  sl018.uSL018.wr.data[2] = 0xAA;
  memcpy(&sl018.uSL018.wr.data[3], key, 6);
  i2c->masterSend(address, sl018.uSL018.wr.len, sl018.uSL018.data);
  timeout.wait(10e-3);
  sl018.uSL018.rd.len = 3;
  i2c->masterReceive(address, sl018.uSL018.rd.len, sl018.uSL018.rd.data);
  errorCode = sl018.uSL018.rd.sts;
  if (errorCode == LOGIN_OK && sl018.uSL018.rd.cmd == CMD_LOGIN
      && (sl018.uSL018.rd.len > 0)) {
    return true;
  }
  return false;
}
#if 0
/*****************************************************************************************
 Read a data block of 16 bytes
 *****************************************************************************************/
bool Csl018::readBlock(u08 block,u08 *_data) {
  data[0] = CMD_READ16;
  i2c->masterSend(address, 0x02, data);
  clearTime();
  while (getTime() < 100000);
  i2c->masterReceive(address, 19, data);
  errorCode = data[2];
  if(errorCode==TAG_OK && data[1] == CMD_READ16 && data[0] > 16) {
    memcpy(_data,&data[3],16);
    return true;
  }
  return false;
}
/*****************************************************************************************
 Read a page of 8 bytes
 *****************************************************************************************/
bool Csl018::readPage(u08 page, u08 *_data) {
  data[0] = CMD_READ4;
  data[1] = page;
  i2c->masterSend(address, 2, data);
  clearTime();
  while (getTime() < 100000);
  i2c->masterReceive(address, 11, data);
  errorCode = data[2];
  if(errorCode==TAG_OK && data[1] == CMD_READ4 && data[0] > 8) {
    memcpy(_data,&data[3],8);
    return true;
  }
  return false;
}

/**	Write 16-u08 block.
 *
 *	The block will be padded with zeroes if the message is shorter
 *	than 15 characters.
 *
 *	@param block Block number
 *	@param message string of 16 characters (binary safe)
 */

void Csl018::writeBlock(u08 block, const c08* message) {
  len = 18;
  cmd = CMD_WRITE16;
  data[0] = block;
  memcpy(&data[1], message, 16);
  data[18] = 0;
  i2c->masterSend(address, len, data);
  while (getTime() < 100000);
  i2c->masterReceive(address, len, data);
  len = data[0];
  cmd = data[1];
  errorCode = data[2];
  if(errorCode==TAG_OK && len > 0) {
    memcpy(_data,&data[3],8);
    return true;
  }
  return false;

}

/**	Write 4-u08 page.
 *
 *	This command is used for Mifare Ultralight tags which have 4 u08 pages.
 *
 *	@param page Page number
 *	@param message String of 4 characters
 */
void Csl018::writePage(u08 page, const c08* message) {
  data[0] = 6;
  data[1] = CMD_WRITE4;
  data[2] = page;
  memcpy((c08*) data + 3, message, 4);
  data[6] = 0;
  transmitData();
}

/** Write master key (key A).
 *
 *	@param sector Sector number
 *	@param key Key value (6 bytes)
 */
void Csl018::writeKey(u08 sector, u08 key[6]) {
  data[0] = 8;
  data[1] = CMD_WRITE_KEY;
  data[2] = sector;
  memcpy(data + 3, key, 6);
  transmitData();
}
#endif
/*****************************************************************************************/
/**	Control red LED on SL018 (not implemented on SL030).
 *
 *	@param on	true for on, false for off
 */
void Csl018::led(bool on) {
  sl018.uSL018.wr.len = 3;
  sl018.uSL018.wr.cmd = CMD_SET_LED;
  sl018.uSL018.wr.data[0] = on;
  i2c->masterSend(address, sl018.uSL018.wr.len, sl018.uSL018.data);
}

/*****************************************************************************************
 * Maps tag types to names.
 *	@param	type numeric tag type
 *	@return	Human-readable tag name as null-terminated string
 ****************************************************************************************/
c08* Csl018::tagName(u08 type) {
  switch (type) {
    case 1:
      return "Mifare 1K";
    case 2:
      return "Mifare Pro";
    case 3:
      return "Mifare UltraLight";
    case 4:
      return "Mifare 4K";
    case 5:
      return "Mifare ProX";
    case 6:
      return "Mifare DesFire";
    default:
      return "";
  }
}

/****************************************************************************************/
char toHex(u08 b) {
  b = b & 0x0f;
  return b < 10 ? b + '0' : b + 'A' - 10;
}

/****************************************************************************************/
void arrayToHex(char *s, u08 array[], u08 len) {
  for (u08 i = 0; i < len; i++) {
    *s++ = toHex(array[i] >> 4);
    *s++ = toHex(array[i]);
  }
  *s = 0;
}

