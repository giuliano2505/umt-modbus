/*
 * File:   platform_i2c.h
 * Author: Parla
 *
 * Created on 20 de junio de 2019, 20:15
 */

#ifndef USE_TMP_H
#define	USE_TMP_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include "platform.h"
#include "platform_gpio.h"
#include "platform_i2c.h"
    

typedef enum
{
    RESOLUTION_9_BITS = 0,
    RESOLUTION_10_BITS = 1,
    RESOLUTION_11_BITS = 2,
    RESOLUTION_12_BITS = 3
} TMP_RESOLUTION_CONFIG;
    
////////////////////////////////////////////////////////////////////////////////
void resetAllSensors (void);    
void configAllSensors(TMP_RESOLUTION_CONFIG, uint8_t *);
uint16_t read_temperature(uint8_t);
void read_sensors(uint16_t *, uint8_t * );
void clean_temp(uint16_t *);
////////////////////////////////////////////////////////////////////////////////
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USE_TMP_H */