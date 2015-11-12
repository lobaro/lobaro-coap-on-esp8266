/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  Bit bang TWI master driver.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the AVR TWI master driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the AVR TWI master.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * 
 * $Date: 2012-06-01 13:03:43 $  \n
 *
 * Copyright (c) 2012, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Author: gary.grewal
 *****************************************************************************/

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

//ESP8266
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO2_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_MTMS_U
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 14
#define I2C_MASTER_SDA_FUNC FUNC_GPIO2
#define I2C_MASTER_SCL_FUNC FUNC_GPIO14

///*! \brief Definition of pin used as SCL. */
//#define SCL 7
//
///*! \brief Definition of pin used as SDA. */
//#define SDA 6
//
///*! \brief Definition of PORT used as SCL. */
//#define PORT_SCL PORTB
///*! \brief Definition of DDR used as SCL. */
//#define DDR_SCL	DDRB
///*! \brief Definition of PIN used as SCL. */
//#define PIN_SCL PINB
///*! \brief Definition of PORT used as SDA. */
//#define PORT_SDA PORTB
///*! \brief Definition of DDR used as SDA. */
//#define DDR_SDA	DDRB
///*! \brief Definition of PIN used as SDA. */
//#define PIN_SDA PINB

/*! \brief Slave 8 bit address (shifted). */
//#define SLAVE_ADDRESS 0x50
extern uint8_t SLAVE_ADDRESS;

//#define READ_SDA() (PIN_SDA & (1 << SDA))
//#define SET_SDA_OUT() DDR_SDA |= (1 << SDA)
//#define SET_SDA_IN() DDR_SDA &= ~(1 << SDA)
//#define READ_SCL() (PIN_SCL & (1 << SCL))?1:0

//ESP8266
//#define READ_SDA() //implemented as function
#define SET_SDA_OUT() //not needed
#define SET_SDA_IN() //not needed
//#define READ_SCL() //implemented as function



#define WRITE 0x0
#define READ 0x1

/*! \brief Delay used to generate clock */
#define DELAY 0

/*! \brief Delay used for STOP condition */
#define SCL_SDA_DELAY 5

//void twi_disable();
void twi_init();
void toggle_scl();
void write_scl(char x);
char twi_start_cond(void);
char send_slave_address(unsigned char read);
char write_data_soft(unsigned char* data, char bytes);
char i2c_write_byte(unsigned char byte);
char read_bytes(unsigned char* data, char bytes);
char i2c_read_byte(unsigned char* data, unsigned char bytes, unsigned char index);
void write_sda( char x);



#endif /* TWI_MASTER_H_ */
