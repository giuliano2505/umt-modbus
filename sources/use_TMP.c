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
void resetAllSensors(){
    for (uint8_t mux = 0 ; mux < 8; mux++){
        mux_select(mux);
        __delay_ms(10);
        i2c_start_com();
        i2c_send(0x00);             //General Call Address
        i2c_send(0b00000110);       //Reset command
        i2c_stop();
        __delay_ms(10);
    }
}
///////////////////////////////////////////////////////////////////////////////
//Function for configuration of a sensors in the selected cable for start conversion.
//This function also returns a byte with the status of the sensor
// 1 if ACK is received, 0 if Its not
///////////////////////////////////////////////////////////////////////////////
uint8_t configSensor(uint8_t sensorNumber,TMP_RESOLUTION_CONFIG config){
    uint8_t W_address = 0b10010000 + (sensorNumber << 1); //Define W_address
    i2c_start_com();
    uint8_t status = i2c_send(W_address);
    if (status == 0) {
        //AKC Received
        i2c_send(0x01);       //Write configurations register
        i2c_send(config << 5);// Set resoution an continuous conversion
    }
    i2c_stop();
    return !status;         // Inverse only for convention
}

///////////////////////////////////////////////////////////////////////////////
//Function for configuration of all sensors in a cable for start conversion.
//This function also returns a byte with the status of the sensors
///////////////////////////////////////////////////////////////////////////////
uint8_t configCable(uint8_t cableNumber,TMP_RESOLUTION_CONFIG config){
    mux_select(cableNumber);
    uint8_t status = 0;
    __delay_ms(10);
    int i;
    for (i = 0; i < 8; i++) {
        status = status | (configSensor(i,config) << i);
        __delay_ms(10);
    }
    return status;    
}

///////////////////////////////////////////////////////////////////////////////
//Function for configuration of all sensor before start making readings.
//This function also modify the status array, for detect bad (or missing) sensors.
///////////////////////////////////////////////////////////////////////////////
void configAllSensors(TMP_RESOLUTION_CONFIG config, uint8_t *status){
    int cablenumber;
    for (cablenumber = 0; cablenumber < 8; cablenumber++) {
        status[cablenumber] = configCable(cablenumber,config);
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
