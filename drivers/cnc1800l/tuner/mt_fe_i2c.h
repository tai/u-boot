/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Montage Technology (Shanghai) Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_i2c.h
*
* Current version:	1.00.00
*
* Description:		i2c function prototype for FE module.
*
* Log:	Description							Version		Date	Author
*		---------------------------------------------------------------------
*		Create								1.00.00	2008.01.15	ChenXiaopeng
*
****************************************************************************/
#ifndef __MT_DEMOD_I2C_H__
#define __MT_DEMOD_I2C_H__
#include "mt_fe_def.h"


 MT_FE_ERR _mt_fe_dmd_set_reg(U8 reg, U8 value);
 MT_FE_ERR _mt_fe_dmd_get_reg(U8 reg, U8 *value);

 MT_FE_ERR _mt_fe_i2c_write(U8 device_addr
	, U8 *p_buf, U16 n_byte, S8 is_enable_repeater);

 MT_FE_ERR _mt_fe_i2c_read(U8 device_addr
	, U8 *p_buf, U16 n_byte, S8 is_enable_repeater);


#endif /* __MT_DEMOD_I2C_H__ */

