/* 
 * File:   modbus_timer.h
 * Author: Cocus
 *
 * Created on 6 de noviembre de 2016, 09:14
 */

#ifndef MODBUS_TIMER_H
#define	MODBUS_TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef enum
{
    MODBUS_TIMER_TIMEOUT_1_5_CHARS = 0,
    MODBUS_TIMER_TIMEOUT_3_5_CHARS = 1,
    MODBUS_TIMER_TIMEOUT_5_CHARS = 2
} MODBUS_TIMER_TIMEOUT;


void InitTimerModbus(void);
void CallFromTimerISR(void);
void SetBaudrate(int Baudrate);
void SetTimerTimeout(MODBUS_TIMER_TIMEOUT timeout);



#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_TIMER_H */

