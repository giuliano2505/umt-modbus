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
void reset_all_sensors(){
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

/**
 * Function for configuration of each sensor installed on a cable
 * @param config TMP_RESOLUTION_CONFIG Configuration to be used
 * @param sensorNumber uint8_t Sensor to be configurated
 * @return uint8_t Sensor status
 */
uint8_t config_sensor(TMP_RESOLUTION_CONFIG config, uint8_t sensorNumber){
    uint8_t W_address = 0b10010000 + (sensorNumber << 1); //Define W_address
    i2c_start_com();
    uint8_t status = i2c_send(W_address);
    if (status == 0) {
        //AKC Received
        i2c_send(CONFIG_REGISTER_ADD);       //Write configurations register
        i2c_send(config);               // Set resoution an continuous conversion
    }
    i2c_stop();
    return !status;         // Inverse only for convention
}

/**
 * Function for the configuration of all installed sensors on a single cable
 * @param config TMP_RESOLUTION_CONFIG Config to be used
 * @param cableNumber uint8_t Cable to be configurated
 * @return uint8_t Cable status
 */
uint8_t config_cable(TMP_RESOLUTION_CONFIG config, uint8_t cableNumber){
    mux_select(cableNumber);
    uint8_t status = 0;
    __delay_ms(10);
    int i;
    for (i = 0; i < 8; i++) {
        status = status | (config_sensor(i,config) << i);
        __delay_ms(10);
    }
    return status;    
}

/**
 * Function for the configuration of all cables installed in a UMT
 * @param status uint8_t vector for sensors status
 * @param config TMP_RESOLUTION_CONFIG Config to be used
 */
void config_all_sensors(TMP_RESOLUTION_CONFIG config, uint8_t *status){
    int cablenumber;
    for (cablenumber = 0; cablenumber < 8; cablenumber++) {
        status[cablenumber] = config_cable(cablenumber,config);
    }
}

/**
 * Function for reading only one sensor, given the sensor number. This function
 *  does not set the mux for cable selection, this must be done before calling it.
 * @param sensorNumber  uint8_t Sensor number to be read
 * @param status uint8_t pointer for sensor status
 * @return uint16_t Temperature in format 0x0AAA (or 0x800, if sensor do not response)
 */
uint16_t read_sensor(uint8_t sensorNumber, uint8_t * status){
    uint16_t temperature = 0;
    uint8_t address = 0b10010000 + (sensorNumber << 1); //Define write address
    //Request of data
    i2c_start_com();
    i2c_send(address);                  //Send W_address to sensors
    i2c_send(TEMP_REGISTER_ADD);        //Send TEMP register write in pointer
    i2c_stop();
    
    __delay_ms(1);
    address += 1;                       //Reading address    
    i2c_start_com();
    *status = !i2c_send(address);
    if (status) {
        temperature = i2c_receive();   //Receive 1 byte from sensors (MSBs)
        temperature <<= 4;             //Rotate 4 bits to left, to make place for LSBs
        temperature += (i2c_receive() >> 4); //Add the 4 LSBs from the received byte to temp
    } else{
        temperature = 0x800;
    }
    i2c_stop();
    
    return temperature;
}
/**
 * Funtion for select a cable and read a sensor
 * @param cableNumber Cable to be selected
 * @param sensorNumber Sensor to be read
 * @return 
 */
uint16_t read_sensor_with_cable(uint8_t cableNumber, uint8_t sensorNumber){
    uint8_t status;
    mux_select(cableNumber);
    __delay_ms(10);
    uint16_t temperature = read_sensor(sensorNumber, &status);
    if(status){
        return temperature;
    } else {
        return 0x800;
    }
}



/**
 * Function for reading all sensors on a UMT
 * @param temperature  uint16_t vector pointer to store temperature readings
 * @return uint8_t number of sensors that awnser back the reading
 */
uint8_t read_all_sensors(uint16_t * temperature){
    uint8_t numberSensorsOK = 0;
    uint8_t status = 0;
    for (int activeCable = 0; activeCable < 8; activeCable++) {
        mux_select(activeCable);
        __delay_ms(10);
        for (int sensorNumber = 0; sensorNumber < 8; sensorNumber++) {
            temperature[activeCable * 8 + sensorNumber] = read_sensor(sensorNumber,&status);
            numberSensorsOK += status;
        }
    }
    return numberSensorsOK;
}