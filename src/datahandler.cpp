/******************************************************************************
SMS Handler

add owner 9 Gary King
add asset 5117 Printer
pair asset 5117 with owner 9 
remove asset 5117 
remove owner 9
set sms1 0836511234
set sms2 0836521234
set sms3 0836531234
get asset 5117
get owner 9
raise alarm
clear alarm

******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "rprintf.h"
#include "sms.h"
#include "smshandler.h"
#include "strtoul.h"
#include "taglocator.h"
#include "drv_modem.h"

#undef DEBUG_SMS_HANDLER
#undef DEBUG_SMS_HANDLER_EXT
#define DEBUG_SMS_HANDLER_ADD

typedef enum
{
  SH_ACTION_NONE = 0,
  SH_ACTION_SET = 1,
  SH_ACTION_GET = 2,
  SH_ACTION_CLR = 3,
  SH_ACTION_ADD = 4,
  SH_ACTION_PAIR = 5,
  SH_ACTION_REMOVE = 6,
  SH_ACTION_RAISE = 7
} shActionType;

typedef enum 
{
  SH_OBJECT_SMS1 = 0,
  SH_OBJECT_SMS2 = 1,
  SH_OBJECT_SMS3 = 2,
  SH_OBJECT_NONE = 3,
  SH_OBJECT_OWNER = 4,
  SH_OBJECT_ASSET = 5,
  SH_OBJECT_ALARM = 6 
} shObjectType;

bool smsInit(void)
{       
        
  return true;
}

bool smsHandle(c08* phoneNumber, c08* cmd)
{
  c08 resp[MAX_SMS_LEN];
  u08 action;
  u08 object;
  c08* str1;
  c08* str2;
  c08* str3;
  c08* str4;
  c08* str5;
  c08* str6;
  c08  cellNr[CELL_NUMBER_LEN];
  u32  ID;
  u32  grdID;
 
  strcpy_P(resp, PSTR("## ERROR ## executing request : "));
  strcat(resp, cmd);

  str1 = strsep (&cmd, " " );
  str2 = strsep (&cmd, " ");
  str3 = strsep (&cmd, " ");
  str4 = strsep (&cmd, " ");
  str5 = strsep (&cmd, " ");
  str6 = strsep (&cmd, " ");
  
  #ifdef DEBUG_SMS_HANDLER
  rprintfProgStrM("\rStr1 = ");
  rprintfProgStrM(str1);
  rprintfProgStrM("\rStr2 = ");
  rprintfProgStrM(str2);
  rprintfProgStrM("\rStr3 = ");
  rprintfProgStrM(str3);
  rprintfProgStrM("\rStr4 = ");
  rprintfProgStrM(str4);
  rprintfProgStrM("\rStr5 = ");
  rprintfProgStrM(str5);
  #endif

  action = SH_ACTION_NONE;
  if (strcmp(str1, "set") == 0) action = SH_ACTION_SET;
  if (strcmp(str1, "get") == 0) action = SH_ACTION_GET;
  if (strcmp(str1, "clear") == 0) action = SH_ACTION_CLR;
  if (strcmp(str1, "add") == 0) action = SH_ACTION_ADD;
  if (strcmp(str1, "pair") == 0) action = SH_ACTION_PAIR;
  if (strcmp(str1, "remove") == 0) action = SH_ACTION_REMOVE;
  object = SH_OBJECT_NONE;
  if (strcmp(str2, "owner") == 0) object = SH_OBJECT_OWNER;
  if (strcmp(str2, "asset") == 0) object = SH_OBJECT_ASSET;
  if (strcmp(str2, "sms1") == 0)  object = SH_OBJECT_SMS1;
  if (strcmp(str2, "sms2") == 0)  object = SH_OBJECT_SMS2;
  if (strcmp(str2, "sms3") == 0)  object = SH_OBJECT_SMS3;
  if (strcmp(str2, "alarm") == 0) object = SH_OBJECT_ALARM;
  
  #ifdef DEBUG_SMS_HANDLER_EXT
  rprintfu08(action);
  rprintfu08(object);
  #endif
 
  switch(action)
  {
    case SH_ACTION_NONE:
      #ifdef DEBUG_SMS_HANDLER
      rprintfProgStrM("\rNo action");
      #endif
      break;
    case SH_ACTION_RAISE:
      if (object == SH_OBJECT_ALARM)
      { 
        // Alarm on
        strcpy(resp, "Raised the alarm ");
      }
      break;
    case SH_ACTION_REMOVE:
      #ifdef DEBUG_SMS_HANDLER_ADD
      rprintfProgStrM("\rAction add.");
      #endif
      if (avr_strtoul(&ID, str3, 10))
      {
        if (object == SH_OBJECT_ASSET)
        {
          if (tlRemoveAssetTag(ID))
          {
            strcpy(resp, "Removed asset with RFID ");
            strcat(resp, str3);
          }
        }
        if (object == SH_OBJECT_OWNER)
        {
          if (tlRemoveGuardTag(ID))
          {
            strcpy(resp, "Removed owner with RFID ");
            strcat(resp, str3);
          }      
        }
      }
      else
      {
        strcat(resp, "Invalid RFID supplied !");
      }
      break;
    case SH_ACTION_ADD:
      #ifdef DEBUG_SMS_HANDLER_ADD
      rprintfProgStrM("\rAction add.");
      #endif
      if (object == SH_OBJECT_OWNER)
      {
        if (avr_strtoul(&ID, str3, 10))
        {
          #ifdef DEBUG_SMS_HANDLER_EXT
          rprintfProgStrM("\rString converted");
          #endif
          if (tlAddGuardTag(ID, str4, str5))    
          {
            strcpy(resp, "Assigned RFID ");
            strcat(resp, str3);
            strcat(resp, " to the new owner ");
            strcat(resp, str4);
            strcat(resp, " ");
            strcat(resp, str5);
          }
        }
      }
      else if (object == SH_OBJECT_ASSET)
      {
        if (avr_strtoul(&ID, str3, 10))
        {
          if (tlAddAssetTag(ID, str3))
          {
            strcpy(resp, "Added new asset RFID : ");
            strcat(resp, str3);
            strcat(resp, " Description : ");
            strcat(resp, str4);
          }
        }
      }
      break;
    case SH_ACTION_PAIR:
      #ifdef DEBUG_SMS_HANDLER_ADD
      rprintfProgStrM("\rAction pair.");
      #endif
      if (object == SH_OBJECT_ASSET)
      {
        if (avr_strtoul(&ID, str3, 10))
        {
          if (avr_strtoul(&grdID, str6, 10))
          {
            #ifdef DEBUG_SMS_HANDLER_EXT
            rprintfProgStrM("\rString converted");
            #endif
            if (tlPairTags(ID, grdID))    
            {
              strcpy(resp, "Paired asset RFID ");
              strcat(resp, str3);
              strcat(resp, " with the owner with RFID ");
              strcat(resp, str6);
            }
          }
        }
      }
      break;
    case SH_ACTION_SET:
      #ifdef DEBUG_SMS_HANDLER_EXT
      rprintfProgStrM("\rSet action");
      #endif
      if (object == SH_OBJECT_SMS1)
      {
        #ifdef DEBUG_SMS_HANDLER
        rprintfProgStrM("\rSet object sms1");
        #endif
        if (tlSetAlarmSMSCellNr(1, str3) == SUCCESS)
        {
          #ifdef DEBUG_SMS_HANDLER
          rprintfProgStrM("\rSet action success");
          #endif
          strcpy(resp, "Cell nr 1 set to ");
          strcat(resp, str3);
        }
      }
      else if (object == SH_OBJECT_SMS2)
      {
        if (tlSetAlarmSMSCellNr(2, str3) == SUCCESS)
        {
          strcpy(resp, "Cell nr 2 set to ");
          strcat(resp, str3);
        }
      }
      else if (object == SH_OBJECT_SMS3)
      {
        if (tlSetAlarmSMSCellNr(3, str3) == SUCCESS)
        {
          strcpy(resp, "Cell nr 3 set to ");
          strcat(resp, str3);
        }
      }
      break;
    case SH_ACTION_GET:
      #ifdef DEBUG_SMS_HANDLER
      rprintfProgStrM("\rGet action");
      #endif
      if (object == SH_OBJECT_SMS1)
      {
        if (tlGetAlarmSMSCellNr(1, cellNr) == SUCCESS)
        {
          strcpy(resp, "Cell nr 1 is currently set to ");
          strcat(resp, cellNr);
        }
      }
      else if (object == SH_OBJECT_SMS2)
      {
        if (tlGetAlarmSMSCellNr(2, cellNr) == SUCCESS)
        {
          strcpy(resp, "Cell nr 2 is currently set to ");
          strcat(resp, str3);
        }
      }
      else if (object == SH_OBJECT_SMS3)
      {
        if (tlGetAlarmSMSCellNr(3, cellNr) == SUCCESS)
        {
          strcpy(resp, "Cell nr 3 is currently set to ");
          strcat(resp, cellNr);
        }
      }
      else if (object == SH_OBJECT_OWNER)
      {
        if (avr_strtoul(&ID, str3, 10))
        {
          if (tlGetGuardName(ID, resp))
          {
            strcpy(resp, " is the owner associated with RFID ");
            strcat(resp, str3);
          }
        }
      }
      else if (object == SH_OBJECT_ASSET)
      {
        if (avr_strtoul(&ID, str3, 10))
        {
          if (tlGetAssetName(ID, resp))
          {
            strcpy(resp, " is the asset associated with RFID ");
            strcat(resp, str3);
          }
        }
      }
      break;
    case SH_ACTION_CLR:
      #ifdef DEBUG_SMS_HANDLER
      rprintfProgStrM("\rClr action");
      #endif
      if (object == SH_OBJECT_SMS1)
      {
        if (tlClrAlarmSMSCellNr(1) == SUCCESS)
        {
          strcpy(resp, "Cell nr 1 cleared.");
        }
      }
      else if (object == SH_OBJECT_SMS2)
      {
        if (tlClrAlarmSMSCellNr(2) == SUCCESS)
        {
          strcpy(resp, "Cell nr 2 cleared.");
        }
      }
      else if (object == SH_OBJECT_SMS3)
      {
        if (tlClrAlarmSMSCellNr(3) == SUCCESS)
        {
          strcpy(resp, "Cell nr 3 cleared.");
        }
      }
      else if (object == SH_OBJECT_ALARM)
      {
        tlClearAlarm();
        strcpy(resp, "Alarm cleared.");
      }
      break;
    default:
      #ifdef DEBUG_SMS_HANDLER
      rprintfProgStrM("\rDefault action");
      #endif
      break;
  }
  MDMSendSMS(phoneNumber, resp);
  return true;
}

