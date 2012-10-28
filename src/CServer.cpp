/*
 * CServer.cpp
 *
 *  Created on: 26 Jan 2009
 *      Author: Wouter
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CModem.h"
#include "CServer.h"
#include "Crtc.h"

extern Crtc rtc;

#define MAX_SERVER_STR_LEN	128

CServer::CServer(CModem *_modem) {
  modem = _modem;
}

void CServer::SetSiteNr(u08 site_nr) {
  SiteNr = site_nr;
}

bool CServer::SetTagInRange(u32 tagNumber) {

  c08 strToServer[MAX_SERVER_STR_LEN];
  CompileStr(strToServer, tagNumber, GAINED);
  return (modem->DataToServer(strToServer));
}

bool CServer::SetTagOutRange(u32 tagNumber) {

  c08 strToServer[MAX_SERVER_STR_LEN];
  CompileStr(strToServer, tagNumber, LOST);
  return (modem->DataToServer(strToServer));
}

bool CServer::SetTagDuress(u32 tagNumber) {

  c08 strToServer[MAX_SERVER_STR_LEN];
  CompileStr(strToServer, tagNumber, DURESS);
  return (modem->DataToServer(strToServer));
}

bool CServer::CompileStr(c08* strToServer, u32 tagNumber, eEvent Event) {

  c08 str[64];
  sprintf(str, "* U(%06d)U", SiteNr);
  strcpy(strToServer, str);
  sprintf(str, " R(%06d)R", (u16) tagNumber);
  strcat(strToServer, str);
  sprintf(str, " T(%s)T",rtc.getTimestamp());
  strcat(strToServer, str);

  switch (Event) {
    case LOST:
      strcat(strToServer, " E(L)E");
      break;
    case GAINED:
      strcat(strToServer, " E(G)E");
      break;
    case DURESS:
      strcat(strToServer, " E(D)E");
      break;
    default:
      break;
  }
  strcat(strToServer, " #");
  return true;
}
