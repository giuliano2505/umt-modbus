/*
 * File:   platform_gpio.c
 * Author: cocus
 *
 * CCreated on 12 de junio de 2019, 15:13
 */

#include "platform_gpio.h"


/** This will expand to (for instance):
 * TRISAbits.RA1 = 0;
 * LATAbits.LATA1 = 0;
 */
#define FILL_INIT_GPIO(GPIO_NAME, PORT, PIN, MODE, DEFAULT)     \
    TRIS##PORT##bits.R##PORT##PIN = MODE;                       \
    LAT##PORT##bits.LAT##PORT##PIN = DEFAULT;                   \

void init_gpio(void)
{
    PLAT_GPIOS(FILL_INIT_GPIO);
}

/** This will expand to (for instance):
 * case PIN_SAMPLE_pin: LATAbits.LAT0 = val; return;
 */
#define FILL_SWITCH_CASES_FOR_GPIO_SET(GPIO_NAME, PORT, PIN, MODE, DEFAULT)   \
    case GPIO_NAME ##_pin:      LAT##PORT##bits.LAT##PORT##PIN = val; return;

void gpio_set(PLAT_GPIO_ENUM gpio, uint8_t val)
{
    switch (gpio)
    {
        PLAT_GPIOS(FILL_SWITCH_CASES_FOR_GPIO_SET);
    }
}
/** This will expand to (for instance):
 * case PIN_SAMPLE_pin: return PORTAbits.RA0;
 */
#define FILL_SWITCH_CASES_FOR_GPIO_READ(GPIO_NAME, PORTNAME, PIN, MODE, DEFAULT)   \
    case GPIO_NAME ##_pin:      return PORT##PORTNAME##bits.R##PORTNAME##PIN;

uint8_t gpio_read(PLAT_GPIO_ENUM gpio)
{
    switch (gpio)
    {
        PLAT_GPIOS(FILL_SWITCH_CASES_FOR_GPIO_READ);
    }   
    return 0; //SI NO FUNCIONA ELIMINARLO
}

void inline mux_select(uint8_t num)
{
    gpio_set(SADD0_pin, (num >> 0) & 0x01);
    gpio_set(SADD1_pin, (num >> 1) & 0x01);
    gpio_set(SADD2_pin, (num >> 2) & 0x01);
}

uint8_t dipswitch_address_read(void)
{
    uint8_t ret = 0;
    ret |= gpio_read(UADD0_pin) << 0;
    ret |= gpio_read(UADD1_pin) << 1;
    ret |= gpio_read(UADD2_pin) << 2;
    ret |= gpio_read(UADD3_pin) << 3;
    
    return ret;
}

uint8_t inline is_silo_full(void)
{
    return gpio_read(SENSSL_pin);
}