#ifndef CRC_H
#define CRC_H

#include "avrlibtypes.h"

#define CRC8INIT	0x00
#define CRC8POLY	0x18

u08 crc8(u08 *data_in, u16 number_of_bytes_to_read);

#endif
