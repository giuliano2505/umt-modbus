/* 
 * File:   modbus_crc.h
 * Author: Cocus
 *
 * Created on 6 de noviembre de 2016, 10:52
 */

#ifndef MODBUS_CRC_H
#define	MODBUS_CRC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
typedef union
{
    uint8_t b[2];
    uint16_t d;
} modbus_crc_union_t;

void crc_restart(modbus_crc_union_t * crc_data);
void crc_calc(uint8_t data, modbus_crc_union_t * crc_data);

uint16_t crc_get_crc(modbus_crc_union_t * crc_data);


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_CRC_H */

