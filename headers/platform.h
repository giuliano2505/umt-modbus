/*
 * File:   platform.h
 * Author: cocus
 *
 * Created on 12 de junio de 2019, 15:13
 */

#ifndef PLATFORM_H
#define	PLATFORM_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <xc.h>
    
/* Crystal frequency */
#define _XTAL_FREQ  16000000UL    
#define CPU_FREQ_NON_CHANGEABLE     1

#define NUMBER_SAMPLES  4

/* PWM config */
#define PWM_PRESCALER_1_1   0
#define PWM_PRESCALER_1_4   1
#define PWM_PRESCALER_1_16  2

#define PWM_ON              1
#define PWM_LOCKED_F        1
#define PWM_PRESCALER_RATIO 1
#define PWM_PRESCALER_REGS  PWM_PRESCALER_1_1
#define PWM_F               15360
#define PWM_DUTY            50

/* UART config */
#define UART_ON             1
#define UART_BRG_HIGH       1
#define UART_BPS            9600


/* GPIOs table */
#define PLAT_GPIOS(d)                                   \
    d(SENSSL,       B, 0,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(UADD0,        B, 1,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(UADD1,        B, 2,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(UADD2,        B, 3,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(UADD3,        B, 4,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(SVCC_EN,      B, 5,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(SADD0,        A, 0,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(SADD1,        A, 1,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(SADD2,        A, 2,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(LED0,         A, 3,  TRISTATE_OUTPUT, VALUE_ON)   \
    d(LED1,         A, 4,  TRISTATE_OUTPUT, VALUE_ON)   \
    d(LED2,         A, 5,  TRISTATE_OUTPUT, VALUE_ON)   \
    d(PWM0,         C, 1,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(I2CSCL,       C, 3,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(I2CSDA,       C, 4,  TRISTATE_INPUT,  VALUE_OFF)  \
    d(RS485DE,      C, 5,  TRISTATE_OUTPUT, VALUE_OFF)  \
    d(UARTTX,       C, 6,  TRISTATE_OUTPUT, VALUE_ON)   \
    d(UARTRX,       C, 7,  TRISTATE_INPUT,  VALUE_OFF)  \


#ifdef CPU_FREQ_NON_CHANGEABLE
#define platform_get_cpu_frequency()    (_XTAL_FREQ)
#else
uint32_t platform_get_cpu_frequency(void);
#endif

void platform_init(void);
void InitTimerSample(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* PLATFORM_H */