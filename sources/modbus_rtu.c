
#include "platform.h"
#include "platform_gpio.h"
#include "usart.h"

#include "modbus_timer.h"
#include "modbus_crc.h"
#include "modbus_rtu.h"







MODBUS_SLAVE Slave;

void CallFromTimerTimeout(void)
{
    switch (Slave.State)
    {
        case MODBUS_STATE_RECEIVING_DATA:
        {
            /* Some data has been received, process it */
            if (Slave.BufferPos >= 4)
            {
                /* At least there is data. Now, check if its valid */

                Slave.Wait = 1; /* Slave is currently busy, not accepting new characters */

                if (ModbusDecoder())
                {
                    ModbusResetSlave();
                    return;
                }

                Slave.State = MODBUS_STATE_PRESEND;

                SetTimerTimeout(MODBUS_TIMER_TIMEOUT_5_CHARS);
            }
            else
            {
                /* Not enough data!, reset all */
                ModbusResetSlave();
            }
            
            break;
        }

        case MODBUS_STATE_PRESEND:
        {
            /* A 5 character time has been passed, so set the bus to 0 */
            SetTransmit(1);

            Slave.State = MODBUS_STATE_SENDING;
            SetTimerTimeout(MODBUS_TIMER_TIMEOUT_3_5_CHARS);
            break;
        }

        case MODBUS_STATE_SENDING:
        {
            /* After 3.5 character time, send the data */
            ModbusSend();

            Slave.State = MODBUS_STATE_POSTSEND;
            SetTimerTimeout(MODBUS_TIMER_TIMEOUT_3_5_CHARS);
            break;
        }

        case MODBUS_STATE_POSTSEND:
        {
            /* Process finished! */
            SetTransmit(0);
            /* Reset all */
            ModbusResetSlave();
        }
    }
}

void CallFromUartRXIsr(char RxChar)
{
    if (Slave.Wait == 1)
    {
        /* The slave is busy (processing another frame) */
        return;
    }

    if (Slave.BufferPos < sizeof(Slave.Modbus_Buffer))
    {
        /* Add this character to the buffer */
        Slave.Modbus_Buffer[Slave.BufferPos] = RxChar;
        /* Increment the write position of the buffer */
        Slave.BufferPos++;
        /* Calculate the CRC */
        crc_calc(RxChar, &Slave.crc);
    }
    else
    {
        Slave.Error = 1;    /* Buffer overflow! */
    }

    /* Reset the timeout (1.5 characters) */
    SetTimerTimeout(MODBUS_TIMER_TIMEOUT_1_5_CHARS);
}

#define CONV2x8TO16(hi,lo) (hi * 256) | lo

MODBUS_EXCEPTIONS ModbusWriteSingleHoldingRegister(uint16_t StartingRegister, uint16_t Value)
{
    MODBUS_EXCEPTIONS   Status = NO_ERROR;
    
    Status = CallbackOnSingleHoldingRegisterUpdate(StartingRegister, Value);
    
    if (Status != NO_ERROR)
    {
        return Status;
    }    
    
    Slave.HoldingRegisters[StartingRegister] = Value;
    Slave.BufferPos += 4; /* Resend the same values */
    
    CallbackOnHoldingRegisterUpdateCorrectly();
    
    return NO_ERROR;
}

MODBUS_EXCEPTIONS ModbusWriteMultipleHoldingRegisters(uint16_t StartingRegister, uint16_t Count)
{
    uint16_t            i;
    uint16_t            TempRegister;
    MODBUS_EXCEPTIONS   Status = NO_ERROR;

    if ((StartingRegister + Count) <= (sizeof(Slave.HoldingRegisters) / sizeof(Slave.HoldingRegisters[0])))
    {
        for(i = StartingRegister; i < StartingRegister + Count; i++)
        {
            TempRegister = Slave.Modbus_Buffer[Slave.BufferPos++] * 256;
            TempRegister |= Slave.Modbus_Buffer[Slave.BufferPos++];

            Status = CallbackOnSingleHoldingRegisterUpdate(i, TempRegister);
            
            if (Status != NO_ERROR)
            {
                return Status;
            }
        }

        Slave.BufferPos = 6;
        
        CallbackOnHoldingRegisterUpdateCorrectly();
        
        return NO_ERROR;
    }
    else
    {
        return ILLEGAL_DATA_ADDRESS;
    }
}




MODBUS_EXCEPTIONS ModbusReadMultipleInputs(uint16_t StartingRegister, uint16_t Count)
{
    if ((StartingRegister + Count) <= (sizeof(Slave.InputRegisters) / sizeof(Slave.InputRegisters[0])))
    {
        Slave.Modbus_Buffer[Slave.BufferPos++] = Count * 2; /* Number of bytes */
        
        while(Count--)
        {
            Slave.Modbus_Buffer[Slave.BufferPos++] = Slave.InputRegisters[StartingRegister] >> 8;
            Slave.Modbus_Buffer[Slave.BufferPos++] = Slave.InputRegisters[StartingRegister] & 0xFF;
            StartingRegister++;
        }   
        
        return NO_ERROR;
    }
    else
    {
        /* Send exception */
        return ILLEGAL_DATA_ADDRESS;
    }
}

MODBUS_EXCEPTIONS ModbusReadMultipleHoldingRegisters(uint16_t StartingRegister, uint16_t Count)
{
    if ((StartingRegister + Count) <= (sizeof(Slave.HoldingRegisters) / sizeof(Slave.HoldingRegisters[0])))
    {
        Slave.Modbus_Buffer[Slave.BufferPos++] = Count * 2; /* Number of bytes */
        
        while(Count--)
        {
            Slave.Modbus_Buffer[Slave.BufferPos++] = Slave.HoldingRegisters[StartingRegister] >> 8;
            Slave.Modbus_Buffer[Slave.BufferPos++] = Slave.HoldingRegisters[StartingRegister] & 0xFF;
            StartingRegister++;
        }  
        
        return NO_ERROR;
    }
    else
    {
        /* Send exception */
        return ILLEGAL_DATA_ADDRESS;
    }
}


void ModbusSend(void)
{
    uint16_t        Pos;
    
    for (Pos = 0; Pos<Slave.BufferPos; Pos++)
    {
        while(BusyUSART());
        WriteUSART(Slave.Modbus_Buffer[Pos]);
        
    }        
}

void SetTransmit(uint8_t state)
{
    gpio_set(RS485DE_pin,state);
}

uint8_t ModbusDecoder(void)
{
    uint16_t            NumberOfRegisters;
    uint16_t            Register;
    uint16_t            i;
    MODBUS_EXCEPTIONS   State;
    

    if (crc_get_crc(&Slave.crc) != 0)
    {
        /* Bad CRC */
        return 1;
    }

    if (Slave.Modbus_Buffer[0] != Slave.Address)
    {
        /* Not this slave's address */
        return 2;
    }

    Slave.BufferPos = 2;    /* Keep the first 2 bytes */    
    
    Register = CONV2x8TO16(Slave.Modbus_Buffer[2], Slave.Modbus_Buffer[3]);
    
    NumberOfRegisters = CONV2x8TO16(Slave.Modbus_Buffer[4], Slave.Modbus_Buffer[5]);
    
    switch (Slave.Modbus_Buffer[1]) /* Function */
    {
        case FUNC_READ_INPUT_REGISTERS:
        {            
            State = ModbusReadMultipleInputs(Register, NumberOfRegisters);
            break;
        }
        
        case FUNC_READ_HOLDING_REGISTERS:
        {
            State = ModbusReadMultipleHoldingRegisters(Register, NumberOfRegisters);
            break;
        }
        
        case FUNC_WRITE_SINGLE_REGISTER:
        {        
            State = ModbusWriteSingleHoldingRegister(Register, NumberOfRegisters);
            break;
        }
        
        case FUNC_WRITE_MULTIPLE_REGISTERS:
        {
            Slave.BufferPos = 7;    /* Keep the first 2 bytes */
            
            State = ModbusWriteMultipleHoldingRegisters(Register, NumberOfRegisters);       
            break;
        }
        
        default:
        {
            /* Illegal function! */
            State = ILLEGAL_FUNCTION;
            break;
        }
    }
    /* Check if the data received has been processed correctly */
    if (State != NO_ERROR)
    {
        Slave.Modbus_Buffer[1] |= FUNC_MASK_ERROR;
        Slave.Modbus_Buffer[2] = State;
        Slave.BufferPos = 3;               
    }
    
    /* Calculate the new CRC */
    crc_restart(&Slave.crc);
	for(i = 0; i < Slave.BufferPos; i++)
    {
		crc_calc(Slave.Modbus_Buffer[i], &Slave.crc);
	}
	i = crc_get_crc(&Slave.crc);    
    
	Slave.Modbus_Buffer[Slave.BufferPos++] = (i >> 8) & 0xff;
	Slave.Modbus_Buffer[Slave.BufferPos++] = i & 0xff;
    
    return 0;
}

void ModbusSetAddress(uint8_t Addr)
{
    Slave.Address = Addr;
}

void ModbusResetSlave(void)
{
    Slave.State = MODBUS_STATE_RECEIVING_DATA;
    Slave.BufferPos = 0;
    Slave.Error = 0;
    Slave.Wait = 0;
    crc_restart(&Slave.crc);
    SetTransmit(0);
}