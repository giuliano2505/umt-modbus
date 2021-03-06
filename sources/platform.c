/*
 * File:   platform.c
 * Author: cocus
 *
 * Created on 12 de junio de 2019, 14:29
 */


#include "platform.h"
#include <timers.h>
#include "platform_gpio.h"
#include "platform_uart.h"
#include "platform_i2c.h"





void pwm_init(void)
{
    // PR2 = 1/(Freq*(4*Tosc*TMR2Prescaler)) - 1
    PR2 = (1.0f / ((double)PWM_F * 4.0f * (double)PWM_PRESCALER_RATIO / (double)platform_get_cpu_frequency())) - 1.0f;

    #define DC      ((double)PWM_DUTY * (double)(_XTAL_FREQ) / ((double)PWM_F * (double)PWM_PRESCALER_RATIO * 100.0f))

    // CCPR2 = PWMDuty/(Tosc*TMR2Prescaler)
    CCPR2L = (DC / 4);
    CCP2CONbits.DC2B = (uint32_t)DC & 0x03;
    

    CCP2CONbits.CCP2M = 0b1100;             // PWM Mode
    T2CONbits.T2CKPS = PWM_PRESCALER_REGS;
    T2CONbits.TMR2ON = 1;                   // ON
}

void InitTimerSample(void){
        /* Sets the Timer0 with 1uS tick base */
#if (_XTAL_FREQ == 16000000)
    OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT &
               T0_PS_1_64 ); 
#elif (_XTAL_FREQ == 4000000)
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT &
               T0_PS_1_16 );
#endif
    WriteTimer0(3035);
    SetTimer0State(0);
}


#ifndef CPU_FREQ_NON_CHANGEABLE
uint32_t platform_get_cpu_frequency(void)
{
    return _XTAL_FREQ;
}
#endif

void platform_init(void)
{
    init_gpio();
    pwm_init();
    //uart_init();
    i2c_init();    
}
