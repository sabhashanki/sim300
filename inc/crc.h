/***************************************************************************
                          	CRC.h
							-------------------
	Begin						:	Wed Oct 11 2000
	Copyright				:	Prism Holdings (Pty)
	Email                	:	heinrichv@prism.co.za | pieterp@prism.co.za
	Authors					:	Pieter Potgieter & Heinrich Vermeulen
	Compiler					:	GCC-AVR
	File Description		:
	
    Revisions :	
	Date     						Author  				Change description									
	==========				======				==================									
	DD/MM/YYYY	|							|		 									
***************************************************************************/

#ifndef CRC_H
#define CRC_H

#include "global.h"

/************************************************************************
* DEFINES
*************************************************************************/
#define CRC_SEED	0

/************************************************************************
* DEFINITIONS
*************************************************************************/


u16 crc16_add(u16 crc, u08 val);
u16 crc16_AddMulti(u16 crc, u08 *val, u08 len);

u08 crc8_buff(u08 *p, u08 l);
u08 crc8(u08 crc, u08 data);


#endif //end header file
