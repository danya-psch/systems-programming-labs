#ifndef GEN_CRC16_H_
#define GEN_CRC16_H_

#include <stdint.h>

#define CRC16 0x8005

uint16_t gen_crc16(const uint8_t *data, uint16_t size);

#endif