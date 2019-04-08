/****************************************************************************
 * MONTAGE PROPRIETARY AND CONFIDENTIAL
 * Montage Technology (Shanghai) Inc.
 * All Rights Reserved
 * --------------------------------------------------------------------------
 *
 * File:				mt_fe_i2c.c
 *
 * Current version:	1.01.00
 *
 * Description:		Define all i2c function for FE module.
 *
 * Log:	Description			Version		Date		Author
 *		---------------------------------------------------------------------
 *		Create				1.00.00		2008.08.21	Rick
 *		Modify				1.01.00		2008.10.13	Rick
 ****************************************************************************/
#include "mt_fe_def.h"

//static CSI2C_HANDLE dmd_i2c_fd =NULL;

/*****************************************************************
 ** Function: _mt_fe_dmd_set_reg
 **
 **
 ** Discription:	write data to demod register
 **
 **
 ** Inputs:
 **
 **	Parametre	type		discription
 **	----------------------------------------------------------
 **	register	U8			register address
 **	data		U8			value to write
 **
 **
 ** Outputs:
 **
 **
 *****************************************************************/

void _mt_fe_dmd_init(unsigned int chipaddress)
{

#if 0
	//if(dmd_i2c_fd!=NULL)
	//{

	// CSI2C_Close( dmd_i2c_fd);
	// dmd_i2c_fd==NULL;
	//}

	CSI2C_Attr i2c_attr;

	dmd_i2c_fd=CSI2C_Open(chipaddress>>1);

	//CSI2C_GetAttr(dmd_i2c_fd, &i2c_attr);

	i2c_attr.speed = I2C_SPEED_400K;
	i2c_attr.write_delayus = 0;
	i2c_attr.subaddr_num = 0;
	i2c_attr.loop = 0;

	CSI2C_SetAttr(dmd_i2c_fd, &i2c_attr);

	printf("xiiiinnnnTUUUUer===========  %d\n",dmd_i2c_fd);
#endif
}


MT_FE_ERR _mt_fe_dmd_set_reg(U8 addr, U8 data)
{
	//tuner_i2c_write(MT_FE_DMD_I2C_ADDR_DS3K,addr,&data,1);
	//tuner_i2c_write(MT_FE_DMD_I2C_ADDR_DS3K,addr,&data,1);

	//unsigned char localBuffer[2];
	//localBuffer[0]=addr;
	//localBuffer[1]=data;

	//CSI2C_Write(dmd_i2c_fd,0,localBuffer,2);

	udelay(100);
	tuner_i2c_write(0x38>>1, addr, 1, &data, 1);

	return MtFeErr_Ok;
}

MT_FE_ERR _mt_fe_dmd_get_reg(U8 addr, U8 *p_buf)
{
	//tuner_i2c_read(MT_FE_DMD_I2C_ADDR_DS3K,addr,p_buf,1);

	//CSI2C_Write(dmd_i2c_fd, 0, &addr, 1);
	//CSI2C_Read(dmd_i2c_fd,0,p_buf,1);

	udelay(100);
	tuner_i2c_read(0x38>>1, addr, 1, p_buf, 1);

	return MtFeErr_Ok;
}

MT_FE_ERR _mt_fe_i2c_write(U8 device_addr, U8 *p_buf, U16 n_byte, S8 is_enable_repeater)
{
	S32 ret;
	U8  tmp[2];

	unsigned char localBuffer[3];
	localBuffer[0]=p_buf[1];
	localBuffer[1]=p_buf[2];
	localBuffer[2]=p_buf[3];
	//localBuffer[3]=p_buf[3];

	//CSI2C_Write(fn_i2c_fd,0,localBuffer,4);

	tuner_i2c_write(0xc0>>1, p_buf[0], 0, p_buf, 5);

	return MtFeErr_Ok;
}

void _mt_sleep(U32 ms)
{
	udelay(ms*10);
}

