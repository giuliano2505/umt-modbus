/* 
 * File:   modbus_rtu.h
 * Author: Cocus
 *
 * Created on 6 de noviembre de 2016, 09:55
 */

#ifndef MODBUS_RTU_H
#define	MODBUS_RTU_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "modbus_crc.h"

//Last address that should be saved to EEPROM
#define MODBUS_HR_LAST_PERSIST_ADD  1    

typedef enum
{
	ILLEGAL_FUNCTION=1,
	ILLEGAL_DATA_ADDRESS=2,
	ILLEGAL_DATA_VALUE=3,
	SLAVE_DEVICE_FAILURE=4,
	ACKNOWLEDGE=5,
	SLAVE_DEVICE_BUSY=6,
	MEMORY_PARITY_ERROR=8,
	GATEWAY_PATH_UNAVAILABLE=10,
	GATEWAY_TARGET_NO_RESPONSE=11,
	TIMEOUT=12,
	NO_ERROR=0
} MODBUS_EXCEPTIONS;


typedef enum
{
    FUNC_NONE=0x00,
    FUNC_READ_COILS=0x01,
    FUNC_READ_DISCRETE_INPUT=0x02,
    FUNC_READ_HOLDING_REGISTERS=0x03,
    FUNC_READ_INPUT_REGISTERS=0x04,
    FUNC_WRITE_SINGLE_COIL=0x05,
    FUNC_WRITE_SINGLE_REGISTER=0x06,
    FUNC_READ_EXCEPTION_STATUS=0x07,
    FUNC_DIAGNOSTICS=0x08,
    FUNC_GET_COMM_EVENT_COUNTER=0x0B,
    FUNC_GET_COMM_EVENT_LOG=0x0C,
    FUNC_WRITE_MULTIPLE_COILS=0x0F,
    FUNC_WRITE_MULTIPLE_REGISTERS=0x10,
    FUNC_REPORT_SLAVE_ID=0x11,
    FUNC_READ_FILE_RECORD=0x14,
    FUNC_WRITE_FILE_RECORD=0x15,
    FUNC_MASK_WRITE_REGISTER=0x16,
    FUNC_READ_WRITE_MULTIPLE_REGISTERS=0x17,
    FUNC_READ_FIFO_QUEUE=0x18,
    FUNC_READ_DEVICE_IDENTIFICATION=43,
    FUNC_MASK_ERROR=0x80
} MODBUS_FUNCTIONS;


typedef enum
{
    MODBUS_STATE_RECEIVING_DATA = 0,
    MODBUS_STATE_PRESEND = 1,
    MODBUS_STATE_SENDING = 2,
    MODBUS_STATE_POSTSEND = 3
} MODBUS_STATE;

//Holding Registers address definitions
typedef enum
{
    MODBUS_HR_BAUDRATE = 0,
    MODBUS_HR_TIME_BW_READINGS = 1,
    MODBUS_HR_SILO_FULL = 2,
    MODBUS_HR_ACTIVE_SENSORS = 3,
    MODBUS_HR_FIRST_STATUS_CABLE = 4,
    MODBUS_HR_FIRST_TEMPERATURE = 12  
} MODBUS_HR_DEF;

typedef struct
{
    uint8_t             Address;

    uint16_t            HoldingRegisters[76];
    uint16_t            InputRegisters[5];

    char                Modbus_Buffer[250];
    uint8_t             BufferPos;
    MODBUS_STATE        State;

    uint8_t             Wait;

    modbus_crc_union_t  crc;

    uint8_t             Error;
} MODBUS_SLAVE;

void SetTransmit(uint8_t state);
void ModbusSend(void);
uint8_t ModbusDecoder(void);
void ModbusSetAddress(uint8_t Addr);
void ModbusResetSlave(void);

void CallFromTimerTimeout(void);
void CallFromUartRXIsr(char RxChar);


extern MODBUS_EXCEPTIONS CallbackOnSingleHoldingRegisterUpdate(uint16_t RegisterNumber, uint16_t NewValue);
extern void CallbackOnHoldingRegisterUpdateCorrectly(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_RTU_H */

