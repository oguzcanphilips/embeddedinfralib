#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void CRC_InitTable(void);
void CRC_Update(uint16_t* crc, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif