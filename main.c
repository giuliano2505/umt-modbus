/*
 * File:   main.c
 * Author: PaRlA
 *
 * Created on 12 de abril de 2022, 13:58
 */

/*
#pragma warning disable 356     //Implicit float to integer
#pragma warning disable 520     //Function Never called  
#pragma warning disable 751     //Arithmetic overflow in constant expression
#pragma warning disable 752     //Conversion to sherter data type
*/
#include <xc.h> 
#include "config_bits.h"
#include <string.h>
#include <stdio.h>
#include <usart.h>
#include <timers.h>
#include <EEP.h>

#include "platform.h"
#include "platform_gpio.h"
#include "modbus_timer.h"
#include "modbus_rtu.h"
#include "platform_uart.h"
#include "platform_i2c.h"
#include "use_TMP.h"

extern MODBUS_SLAVE Slave;
uint8_t takeReadingFlag = 0;
uint16_t secondsCounter = 0;

void interrupt isr(void)
{
    
    //We should implement here a flag for start a reading when T2 finish
    if (INTCON1bits.TMR0IF)
    {
        WriteTimer0(3035);
        
        if (secondsCounter > Slave.HoldingRegisters[MODBUS_HR_TIME_BW_READINGS]){
            //Reset seconds counter
            secondsCounter = 0;
            takeReadingFlag = 1;
        } else {
            secondsCounter++;
        }        
 
        INTCON1bits.TMR0IF = 0;
    }
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
    /*Time between readings*/
    Slave.HoldingRegisters[MODBUS_HR_TIME_BW_READINGS] = 60; 
    
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
            break;            
        }
        case MODBUS_HR_TIME_BW_READINGS:
        {
            if ((NewValue >= 60) && (NewValue <= 21600)){
                break;
            } else {
                return ILLEGAL_DATA_VALUE;
            }
        }
        case MODBUS_HR_TAKEREADING:
        {
            if (NewValue > 0){
                secondsCounter = 0;
                takeReadingFlag = 1;
                break;
            } else {
                return ILLEGAL_DATA_VALUE;
            }
        }
        default:
        {
            return ILLEGAL_DATA_ADDRESS;
        }
    }
    return NO_ERROR;
    
}

void CallbackOnHoldingRegisterUpdateCorrectly(void)
{
    StoreConfigParametersToEEPROM();
    __delay_ms(10);
    /* Apply the new parameters */
    InitUSART(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);    
    SetBaudrate(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    //We have to read address from DIPSwitchs
    ModbusSetAddress(read_DIPSwitch_address());  
}

void InitializeHoldingRegisters(void){
    Slave.HoldingRegisters[MODBUS_HR_ACTIVE_SENSORS] = 0;
    for (int i = 0; i < 8; i++) {
        Slave.HoldingRegisters[MODBUS_HR_FIRST_STATUS_CABLE + i] = 0;
    }
    for (int i = 0; i < 64; i++) {
        Slave.HoldingRegisters[MODBUS_HR_FIRST_TEMPERATURE + i] = 0x800;
    }
}



void X10msDelay(int n){
    int i;
    for (i = 0; i < n; i++) {
        __delay_ms(10);
    }
}


void TakeReading(void){
    TMP_RESOLUTION_CONFIG TMPconfig = RESOLUTION_12_BITS;
    int16_t temperatures[8];
    uint8_t status = 0;
    uint8_t totalActiveSensors = 0;
    gpio_set(SVCC_EN_pin, 0); //Turn on Sensor power
    gpio_set(LED1_pin, 0);
    __delay_ms(10);
    reset_all_sensors();
    config_all_sensors(TMPconfig);
    X10msDelay(100);
    int cableNumber;
    int16_t maxSiloTemperature = 0x800;                //Define a minimum temperature
    int16_t maxCableTemperature = 0x800;                //Define a minimum temperature
    for (cableNumber = 0; cableNumber < 8; cableNumber++) {
        totalActiveSensors += ReadAllSensorsOnCable(cableNumber,&status,temperatures);
        Slave.HoldingRegisters[MODBUS_HR_FIRST_STATUS_CABLE + cableNumber] = status;
        int sensorNumber;
        for (sensorNumber = 0; sensorNumber < 8; sensorNumber++) {
            Slave.HoldingRegisters[MODBUS_HR_FIRST_TEMPERATURE + 8 * cableNumber + sensorNumber] = temperatures[sensorNumber];
            if(maxCableTemperature < temperatures[sensorNumber]){
                maxCableTemperature = temperatures[sensorNumber];
            }
        }
        if(maxSiloTemperature < maxCableTemperature){
            maxSiloTemperature = maxCableTemperature;
        }
        Slave.HoldingRegisters[MODBUS_HR_FIRST_MAX_CABLE + cableNumber] = maxCableTemperature;
        maxCableTemperature = 0x800;
    }
    Slave.HoldingRegisters[MODBUS_HR_MAX_SILO] = maxSiloTemperature;
    Slave.HoldingRegisters[MODBUS_HR_ACTIVE_SENSORS] = totalActiveSensors;
    
    gpio_set(SVCC_EN_pin, 1); //Turn off Sensor power
    gpio_set(LED1_pin, 1);
    
}



void main(void) {
    //Init all needed (GPIOs, I2C, ..)
    platform_init();

    //FOR DISABLING A UMT SET ADDRESS == 0
    if(read_DIPSwitch_address() == 0){
        gpio_set(LED2_pin,0);
        while(1);
    }
        
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    ReadParametersFromEEPROM();

    InitUSART(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    InitTimerModbus();
    InitTimerSample();
    SetBaudrate(Slave.HoldingRegisters[MODBUS_HR_BAUDRATE]);
    ModbusResetSlave();
    ModbusSetAddress(read_DIPSwitch_address());
    InitializeHoldingRegisters();
    SetTimer0State(1);
    takeReadingFlag = 1;
    while (1) {
        gpio_set(LED0_pin, 0);
        X10msDelay(100);
        gpio_set(LED0_pin, 1);
        X10msDelay(100);
        Slave.HoldingRegisters[MODBUS_HR_SILO_FULL] = is_silo_full();
        if (takeReadingFlag) {
            gpio_set(LED1_pin,0);
            TakeReading();
            takeReadingFlag = 0;
            Slave.HoldingRegisters[MODBUS_HR_TAKEREADING] = 0;
            gpio_set(LED1_pin,1);
        }
    }
}

