/*
 * File:   platform_i2c.c
 * Author: Parla
 *
 * Created on 20 de junio de 2019, 20:15
 */

#include "platform_i2c.h"

///////////////////////////////////////////////////////////////////////////////
//Initialization of SSP module on I2C mode, on XC8 compiler
//for PIC18F242 working at 100Kbps with a 4 MHz Oscilator
///////////////////////////////////////////////////////////////////////////////
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
    
    SSPADD=9;                   //For 100KHz clock frequency
}

///////////////////////////////////////////////////////////////////////////////
//Wait function 
//Wait until there is no comunication is on curse
///////////////////////////////////////////////////////////////////////////////
 
void i2c_wait(void){
    while((SSPCON2 & 0b00011111) || (SSPSTAT & 0b00000100));
}

///////////////////////////////////////////////////////////////////////////////
//Starts a I2C comunication
///////////////////////////////////////////////////////////////////////////////
void i2c_start_com(void) {
    i2c_wait();             //Wait for conditions
    SSPCON2bits.SEN = 1;    //Set start comunication bit
}

///////////////////////////////////////////////////////////////////////////////
//Restart a I2C comunication
///////////////////////////////////////////////////////////////////////////////
void i2c_reset_com(void){
    i2c_wait();             //Wait for conditions
    SSPCON2bits.RSEN = 1;   //Set restart comunication bit
}

///////////////////////////////////////////////////////////////////////////////
//Stop a I2C comunication
///////////////////////////////////////////////////////////////////////////////
void i2c_stop(void) {
    i2c_wait();             //Wait for conditions
    SSPCON2bits.PEN = 1;    //Set stop comunication bit
}

///////////////////////////////////////////////////////////////////////////////
//Send data (a byte) to the slave
///////////////////////////////////////////////////////////////////////////////
uint8_t i2c_send(uint8_t data) {
    i2c_wait();             //Wait for conditions
    SSPBUF = data;          //Write data to send on SSPBUF (SSP buffer)
    i2c_wait();             //Wait por all data transmited
    return  ACKSTAT;        //Return 0 if Tx was OK
}

///////////////////////////////////////////////////////////////////////////////
//Receive data (a byte) to the slave
///////////////////////////////////////////////////////////////////////////////
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
