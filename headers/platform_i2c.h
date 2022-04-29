/*
 * File:   platform_i2c.h
 * Author: Parla
 *
 * Created on 20 de junio de 2019, 20:15
 */

#ifndef PLATFORM_I2C_H
#define	PLATFORM_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#include "platform.h"
   
void i2c_init(void);                        //Function for init I2C comunication
void i2c_wait(void);                        //Wait for conditions
void i2c_start_com(void);                   //Starts I2C comunication
void i2c_reset_com(void);                   //Restarts I2C comunication 
void i2c_stop(void);                        //Stops I2C comunication
uint8_t i2c_send(uint8_t data); //Write (or send) to the slave
uint8_t i2c_receive(void);            //Read (or receive) from the slave

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* PLATFORM_I2C_H */