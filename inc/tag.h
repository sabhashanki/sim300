/*****************************************************************************/
#ifndef TAG_H
#define TAG_H
/*****************************************************************************/
#include "global.h"
#include "sms.h"
/*****************************************************************************/
#define RFID_TAG_LEN      11
#define MAX_NAME_LEN      32
/*****************************************************************************/
typedef enum
{
  TAG_UNUSED,
  TAG_IN_RANGE,
  TAG_OUT_RANGE,
  TAG_DUR
} tagState;
/*****************************************************************************/
typedef struct
{
  u32       RFID;
  tagState  State;
  bool      Logged;
  bool      Displayed;
  u08       SignalDuress;
  c08       Name[MAX_NAME_LEN];
} sTag;
/*****************************************************************************/
u08 tagInit(sTag* this);
u08 tagSetID(sTag* this, u32 RFID);
u08 tagSetName(sTag* this, c08* Name);
u08 tagGetName(sTag* this, c08* Name);
u08 tagService(sTag* this);
/*****************************************************************************/
#endif

