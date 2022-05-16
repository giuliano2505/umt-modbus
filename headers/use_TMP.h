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
    RESOLUTION_9_BITS = (0 << 5),
    RESOLUTION_10_BITS = (1 << 5),
    RESOLUTION_11_BITS = (2 << 5),
    RESOLUTION_12_BITS = (3 << 5)
} TMP_RESOLUTION_CONFIG;

#define TEMP_REGISTER_ADD 0x00
#define CONFIG_REGISTER_ADD 0x01
#define TLOW_REGISTER_ADD 0x02
#define THIGH_REGISTER_ADD 0x03

void reset_all_sensors(void);
void config_all_sensors(TMP_RESOLUTION_CONFIG);
uint16_t read_sensor(uint8_t sensorNumber, uint8_t * status);
uint16_t read_sensor_with_cable(uint8_t , uint8_t , uint8_t * );
uint8_t ReadAllSensorsOnCable(uint8_t cableNumber, uint8_t * status, uint16_t * temperature);
uint8_t read_all_sensors(uint16_t * );

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* USE_TMP_H */