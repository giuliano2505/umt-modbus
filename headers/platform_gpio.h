/*
 * File:   platform_gpio.h
 * Author: cocus
 *
 * Created on 12 de junio de 2019, 15:13
 */

#ifndef PLATFORM_GPIO_H
#define	PLATFORM_GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include "platform.h"
    
#define TRISTATE_INPUT      1
#define TRISTATE_OUTPUT     0

#define VALUE_OFF           0
#define VALUE_ON            1

#define FILL_GPIO_ENUM(GPIO_NAME, PORT, PIN, MODE, DEFAULT)   \
    GPIO_NAME ##_pin,
typedef enum
{
    PLAT_GPIOS(FILL_GPIO_ENUM)
} PLAT_GPIO_ENUM;



void init_gpio(void);

void gpio_set(PLAT_GPIO_ENUM gpio, uint8_t val);
uint8_t gpio_read(PLAT_GPIO_ENUM gpio);

void inline mux_select(uint8_t num);


uint8_t dipswitch_address_read(void);

uint8_t inline is_silo_full(void);


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* PLATFORM_GPIO_H */

