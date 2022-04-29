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
    

////////////////////////////////////////////////////////////////////////////////
void reset_sensors (void);    
void config_sensors(uint8_t *status);
uint16_t read_temperature(uint8_t add);
void read_sensors(uint16_t *temp, uint8_t *status );
void clean_temp(uint16_t *temp);
void delaymsX10(uint8_t multi);   
////////////////////////////////////////////////////////////////////////////////
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USE_TMP_H */