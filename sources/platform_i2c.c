/*
 * File:   platform_i2c.c
 * Author: Parla
 *
 * Created on 20 de junio de 2019, 20:15
 */

#include "platform_i2c.h"


/**
 * Initialization of SSP module on I2C mode, at 100 KHZ with 4 or 16 MHZ clock
 */
void i2c_init(void){
    //SDA and SCL are both defined on gpio init
    SSPSTATbits.SMP = 1;        //Slew rate control disabled for Standard Speed mode (100 kHz and 1 MHz)
    SSPSTATbits.CKE = 0;        //Disable SMBus specific inputs
    
    SSPCON1bits.WCOL = 0;       //Write Collision Detect bit
    SSPCON1bits.SSPOV = 0;      //Receive Overflow Indicator bit
    SSPCON1bits.SSPEN = 1;      //Enables the serial port and configures the SDA and SCL pins as the serial port pins
    SSPCON1bits.CKP = 0;        //Unused in I2C Master mode, cleared for security
    SSPCON1bits.SSPM = 0b1000;  //I2C Master mode, clock = FOSC / (4 * (SSPADD+1))
    
    SSPCON2=0b00000000;         //No I2C communication started
    
#if (_XTAL_FREQ ==  4000000)  
    SSPADD = 9;                   //For 100KHz clock frequency with Fosc = 4MHz
#elif (_XTAL_FREQ == 16000000)
    SSPADD = 39;                  //For 100KHz clock frequency with Fosc = 16MHz
#endif

}

/**
 * Low level function for waiting until there is no comunication on curse
 */
void i2c_wait(void){
    while((SSPCON2 & 0b00011111) || (SSPSTAT & 0b00000100));
}

/**
 * Low level function for starting a I2C comunication
 */
void i2c_start_com(void) {
    i2c_wait();             //Wait for conditions
    SSPCON2bits.SEN = 1;    //Set start comunication bit
}

/**
 * Low level function for reset a I2C comunication
 */
void i2c_reset_com(void){
    i2c_wait();             //Wait for conditions
    SSPCON2bits.RSEN = 1;   //Set restart comunication bit
}

/**
 * Low level function for stop a I2C comunication
 */
void i2c_stop(void) {
    i2c_wait();             //Wait for conditions
    SSPCON2bits.PEN = 1;    //Set stop comunication bit
}

/**
 * Low level function for send a byte by I2C
 * @param data uint8_t Data to be transmited
 * @return uint8_t Acknolege 0 if OK
 */
uint8_t i2c_send(uint8_t data) {
    i2c_wait();             //Wait for conditions
    SSPBUF = data;          //Write data to send on SSPBUF (SSP buffer)
    i2c_wait();             //Wait por all data transmited
    return  SSPCON2bits.ACKSTAT;        //Return 0 if Tx was OK
}

/**
 * Low level function for receive a byte from a I2C slave
 * @return uint8_t byte received
 */
uint8_t i2c_receive(void){
 
    uint8_t read_byte;    //Variable to read 
 
    i2c_wait();             //Wait for conditions
    SSPCON2bits.RCEN = 1;   //Restart I2C comunication
    i2c_wait();             //Wait for conditions
    read_byte = SSPBUF;     //Read buffer
    i2c_wait();             //Wait for conditions
    SSPCON2bits.ACKDT = 0;  //Set Acknowledge Data bit (Not Acknowledge)
    SSPCON2bits.ACKEN = 1;  //Set Acknowledge Sequence Enable bit 
    return read_byte;
}
