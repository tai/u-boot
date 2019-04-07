/** -----------------------------------------------------------------------
 *  \file   tuner_i2c.h
 *  \brief  Tuner I2C control head file
 *  \author  
 *  \date    
 *  \note   Following items should be observed:
 *          -#  notice point A  
 --------------------------------------------------------------------------*/

#ifndef		_TUNER_I2C_H_
#define		_TUNER_I2C_H_

#include <common.h>
#include <config.h>
#include <command.h>
#include <i2c.h>

/* I2C Device Dddress */
#define	STV0288_DEMOD_I2C_ADDR	0x60
#define	STV0288_TUNER_I2C_ADDR	0x68

#define	P10023_DEMOD_I2C_ADDR	0x0C
#define	P10023_TUNER_I2C_ADDR	0x60

#define I2C_COMMAND_LEN 	6
int tuner_i2c_init(void);
int tuner_i2c_read(unsigned int address, int subaddr, int alen,
		   unsigned char *buffer, unsigned int num);
int tuner_i2c_write(unsigned int address, int subaddr, int alen,
		    unsigned char *buffer, unsigned int num);
int tuner_write_mask(unsigned int address, int subaddr, int alen, int mask,
		     unsigned char data);

#endif /*  */
