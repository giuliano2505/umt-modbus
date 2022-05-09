/*
 * File:   main.c
 * Author: PaRlA
 *
 * Created on 12 de abril de 2022, 13:58
 */
#include "config_bits.h"
#include <string.h>
#include <stdio.h>
#include <usart.h>
#include <EEP.h>

#include "platform.h"
#include "platform_gpio.h"
#include "modbus_timer.h"
#include "modbus_rtu.h"
#include "platform_uart.h"
//#include "platform_i2c.h"
//#include "use_TMP.h"

extern MODBUS_SLAVE Slave;

void interrupt isr(void)
{
    
    //We should implement here a flag for start a reading when T2 finish
    
    //This is for modbus use
    if (PIR1bits.TMR1IF)
    {
        /* Modbus */
        CallFromTimerISR();

        PIR1bits.TMR1IF = 0;
    }    
    
    if (PIR1bits.RCIF)
    {
        /* Modbus */
        CallFromUartRXIsr(RCREG);
    }
}


void StoreConfigParametersToEEPROM(void)
{
    uint8_t Idx = 0;
    uint8_t RegIdx;
    
    Write_b_eep(Idx++, 0);  /* Valid data in EE */
    
    for (RegIdx = 0; RegIdx < (MODBUS_HR_LAST_PERSIST_ADD + 1); RegIdx++)
    {
        Write_b_eep(Idx++, Slave.HoldingRegisters[RegIdx] >> 8);
        Busy_eep();
        Write_b_eep(Idx++, Slave.HoldingRegisters[RegIdx]);        
        Busy_eep();
    }  
}

void StoreDefaultParametersToEEPROM(void)
{    
    /* Baudrate */
    Slave.HoldingRegisters[MODBUS_HR_BAUDRATE] = 9600;  
    
    StoreConfigParametersToEEPROM();
}

void ReadParametersFromEEPROM(void)
{
    uint8_t Idx = 0;
    uint8_t RegIdx;
    
    if (Read_b_eep(Idx++) == 0xFF)
    {
        StoreDefaultParametersToEEPROM();
        
        return;
    }

    for (RegIdx = 0; RegIdx < (MODBUS_HR_LAST_PERSIST_ADD + 1); RegIdx++)
    {
        Slave.HoldingRegisters[RegIdx] = Read_b_eep(Idx++) * 256;
        Slave.HoldingRegisters[RegIdx] |= Read_b_eep(Idx++);      
    }
}

MODBUS_EXCEPTIONS CallbackOnSingleHoldingRegisterUpdate(uint16_t RegisterNumber, uint16_t NewValue)
{
    switch (RegisterNumber)
    {
        case MODBUS_HR_BAUDRATE:
        {
            switch (NewValue)
            {
                case 19200:
                case 9600:
                case 2400:
                case 1200:
                {
                    break;
                }
                default:
                {
                    return ILLEGAL_DATA_VALUE;
                }
            }            
        }
    }

    return NO_ERROR;
}

void CallbackOnHoldingRegisterUpdateCorrectly(void)
{
    StoreConfigParametersToEEPROM();
    
    /* Apply the new parameters */
    InitUSART(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);    
    SetBaudrate(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    //We have to read address from DIPSwitchs
    ModbusSetAddress(Slave.HoldingRegisters[MODBUS_HR_ADDRESS]);  
}



void X10msDelay(int n){
    int i;
    for (i = 0; i < n; i++) {
        __delay_ms(10);
    }
}




void main(void) {
    //Init all needed (GPIOs, UART, I2C, ..)
    platform_init();
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    ReadParametersFromEEPROM();

    InitUSART(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    InitTimer();
    SetBaudrate(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    ModbusResetSlave();
    ModbusSetAddress(Slave.HoldingRegisters[MODBUS_HR_ADDRESS]);
    
    while (1) {
        gpio_set(LED0_pin, 0);
        X10msDelay(100);
        gpio_set(LED0_pin, 1);
        X10msDelay(100);
        
        //printf("%d \n",read_DIPSwitch_address());
        /*
        gpio_set(SVCC_EN_pin, 0); //Turn on Sensor power
        __delay_ms(10);
        reset_sensors(); //Software reset of all cables for security
        __delay_ms(10);
        config_sensors(status); //Config all sensors with 12 bits resolution

        gpio_set(SVCC_EN_pin, 1); //Turn off Sensor power
         */
    }
}
