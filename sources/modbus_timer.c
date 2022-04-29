
#include "platform.h"
#include "timers.h"
#include "modbus_timer.h"
#include "modbus_rtu.h"

#define SetTimer1State(x) { T1CONbits.TMR1ON = x; }


int TimerLoad_1_5_Chars = 0;
int TimerLoad_3_5_Chars = 0;
int TimerLoad_5_Chars = 0;

MODBUS_TIMER_TIMEOUT CurrentTimeoutMode = MODBUS_TIMER_TIMEOUT_1_5_CHARS;

void InitTimer(void)
{
    /* Sets the Timer1 with 1uS tick base */
#if (_XTAL_FREQ == 16000000)
    OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT &
               T1_PS_1_4 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);    
#elif (_XTAL_FREQ == 8000000)
    OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT &
               T1_PS_1_2 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
#elif (_XTAL_FREQ == 4000000)
    OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT &
               T1_PS_1_1 & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
#endif
    SetTimer1State(0);
}

void CallFromTimerISR(void)
{
    //WriteTimer1()
    SetTimer1State(0);  /* disables the timer */
    CallFromTimerTimeout();
}

void SetBaudrate(int Baudrate)
{
    float BitsPerTick = 1000000;
    BitsPerTick /= Baudrate;

    TimerLoad_1_5_Chars = 65535 - (BitsPerTick * 14);
    TimerLoad_3_5_Chars = 65535 - (BitsPerTick * 32);
    TimerLoad_5_Chars = 65535 - (BitsPerTick * 45);
}

void SetTimerTimeout(MODBUS_TIMER_TIMEOUT timeout)
{
    CurrentTimeoutMode = timeout;

    if (timeout == MODBUS_TIMER_TIMEOUT_1_5_CHARS)
    {
        WriteTimer1(TimerLoad_1_5_Chars);
    }
    else if (timeout == MODBUS_TIMER_TIMEOUT_3_5_CHARS)
    {
        WriteTimer1(TimerLoad_3_5_Chars);
    }
    else if (timeout == MODBUS_TIMER_TIMEOUT_5_CHARS)
    {
        WriteTimer1(TimerLoad_5_Chars);
    }

    SetTimer1State(1);  /* enable the timer */
}