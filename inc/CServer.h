/*
 * CServer.h
 *
 *  Created on: 26 Jan 2009
 *      Author: Wouter
 */

#ifndef CSERVER_H_
#define CSERVER_H_

#include "avrlibtypes.h"
#include "CModem.h"

typedef enum
{
	LOST,
	GAINED,
	DURESS
} eEvent;

class CServer {
private:
  u08  SiteNr;
	Cmodem *modem;
public:
	CServer(Cmodem *_modem);
	bool SetTagInRange(u32 tagNumber);
	bool SetTagOutRange(u32 tagNumber);
	bool SetTagDuress(u32 tagNumber);
	bool CompileStr(c08* strToServer, u32 tagNumber, eEvent Event);
	void SetSiteNr(u08 site_id);
};

#endif /* CSERVER_H_ */
