/*
 * File:   platform_i2c.c
 * Author: Parla
 *
 * Created on 20 de junio de 2019, 20:15
 */

#include "use_TMP.h"
#include "platform.h"

///////////////////////////////////////////////////////////////////////////////
//Function for reset all sensors using General Calls, after power on the cables.
///////////////////////////////////////////////////////////////////////////////
void resetSensors(){
    for (uint8_t mux = 0 ; mux < 8; mux++){
        mux_select(mux);
        __delay_ms(10);
        i2c_start_com();
        i2c_send(0x00);             //General Call Address
        i2c_send(0b00000110);       //Reset command
        i2c_stop();
    }
}
///////////////////////////////////////////////////////////////////////////////
//Function for configuration of all sensor before start making readings.
//This function also modify the status array, for detect bad (or missing) sensors.
///////////////////////////////////////////////////////////////////////////////
void configSensors(uint8_t *status){
    uint8_t auxstatus = 0;
    uint8_t W_address = 0;
        for (uint8_t mux = 0 ; mux < 8; mux++){
            mux_select(mux);                                    //Select cable
            auxstatus = 0;
            __delay_ms(10);                     
            for(uint8_t sensor = 0 ; sensor < 8 ; sensor++){
                W_address = 0b10010000 + (sensor << 1); //Define W_address
                i2c_start_com();
                //Make a right rotation, so the LSB of auxstatus is Sensor#0
                auxstatus >>= 1;
                //"i2c_read" give 0x01 if no sensors, this line asign it to MSB
                auxstatus += (i2c_send(W_address) << 7);
                i2c_send(0x01);     //Write configurations register
                i2c_send(0x60);     //Set 12bits resolution, continous conversion
                i2c_stop();
            }
            //Asign to status array the complement of auxstatus.
            //So 0 means no sensor, 1 means sensor present.
            status[mux] = ~auxstatus;           
    }
}


///////////////////////////////////////////////////////////////////////////////
//Function to read only one sensor, with the given W_address. This function no
//Select the mux, so it have to be configurated before call it.
//It does not return the TMP100 format "0xAAA0", but "0xAAA".
///////////////////////////////////////////////////////////////////////////////
uint16_t read_temperature(uint8_t add){
    uint16_t temp = 0;
    //Request of data
    i2c_start_com();
    i2c_send(add);          //Send W_address to sensors
    i2c_send(0x00);         //Send TEMP register write in pointer
    i2c_stop();
    //Set original address (Write_add) to Read_add
    add += 1;
    i2c_start_com();
    i2c_send(add);          //Send R_address to sensors
    temp = i2c_receive();   //Receive 1 byte from sensors (MSBs)
    temp <<= 4;             //Rotate 4 bits to left, to make place for LSBs
    temp += (i2c_receive() >> 4); //Add the 4 LSBs from the received byte to temp
    i2c_stop();
    return temp;
}
///////////////////////////////////////////////////////////////////////////////
//Function to read all sensors on the UMT, It takes the status array so does 
//not read all sensors unnecessarily. Modify the temperature array with the readed
//temperature or, if sensor is not present set it to 0x800.
//This is only for security, the status array has data about operative sensors too.
///////////////////////////////////////////////////////////////////////////////
void read_sensors(uint16_t *temp, uint8_t *status ){
    uint8_t index = 0;
    for (uint8_t mux = 0 ; mux < 8; mux++){
        mux_select(mux);                                //Select cable
        __delay_ms(10);
        for(uint8_t sensor = 0 ; sensor < 8 ; sensor++){
            uint8_t W_address = 0b10010000 + (sensor << 1); //Define W_address
            if (status[mux] & (1 << sensor)){
                //If status bit is 1, read temperature
                temp[index] += read_temperature(W_address);
            }
            else{
                //If status bit is 0, set 0x800
                temp[index] += 0x800; // -128C (Irreal temperature)
            }
            index++;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
//Function to initialize on 0 all temperature.
//Here only to make code easier to understand later.
///////////////////////////////////////////////////////////////////////////////
void clean_temp(uint16_t *temp){
    for (uint8_t i = 0; i < 64; i++){
        temp[i] = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////
//Function to make bigger delays (>100ms aprox).
//It does not consider the time that take the "for" loop.
//The aplication does not need that level of precision.
//Delay time is time*multi.
///////////////////////////////////////////////////////////////////////////////
void delaymsX10(uint8_t multi){
    for(int i = 0; i < multi; i++){
        __delay_ms(10);
    }
}