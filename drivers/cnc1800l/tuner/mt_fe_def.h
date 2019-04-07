/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Montage Technology (Shanghai) Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_def.h
*
* Current version:	2.01.00
*
* Description:
*
* Log:	Description							Version		Date	Author
*		---------------------------------------------------------------------
*		Create								1.00.00	2008.01.15	ChenXiaopeng
*		Modify								2.00.00	2008.05.12	ChenXiaopeng
*		Modify								2.01.00	2008.05.15	ChenXiaopeng
*
****************************************************************************/
#ifndef __MT_FE_DEF_H__
#define __MT_FE_DEF_H__

//#include <dbgprint.h>					/*	Public definitions for debug print		*/


#ifdef __cplusplus
extern "C" {
#endif


#define MT_FE_VERSION						0x020402
#define MT_FE_DEBUG							1			/*	0 off, 1 on									*/


#define SIMAP_AUTO_ENABLE					1			/*	enable SI Map auto detect					*/
#define SIMAP_AUTO_EXTEND					0			/*												*/
#define ERR_PIN_ENABLE						0			/*	enable it_lock pin as m_err pin				*/



#define MT_FE_DMD_I2C_ADDR_DD2K				0x38		/* Demodulator address							*/
#define MT_FE_TN_I2C_ADDR					0xc0		/* Tuner address								*/


/*	DEMODULATOR SUPPORTED	*/
#define MT_FE_DMD_SUPPORT_DD2KA0			0
#define MT_FE_DMD_SUPPORT_DD2KA1			1



/*	TUNERS SUPPORTED	*/
#define MT_FE_TN_SUPPORT_TD1611ALF4			0
#define MT_FE_TN_SUPPORT_SHARP6093			0
#define MT_FE_TN_SUPPORT_SHARP2093			0
#define MT_FE_TN_SUPPORT_TDTCG0X2D			0
#define MT_FE_TN_SUPPORT_TDQB3_601A			0
#define MT_FE_TN_SUPPORT_TDQG4006A			0
#define MT_FE_TN_SUPPORT_MxL_5007T			1
#define MT_FE_TN_SUPPORT_TDAD3_C01A			0


/*	TUNERS	ID			*/
#define MT_FE_TN_ID_TD1611ALF4				0x01
#define MT_FE_TN_ID_SHARP6093				0x11
#define MT_FE_TN_ID_SHARP2093				0x12
#define MT_FE_TN_ID_TDTCG0X2D				0x21
#define MT_FE_TN_ID_TDQB3_601A				0x31
#define MT_FE_TN_ID_TDQG4006A				0x32
#define MT_FE_TN_ID_TDAD3_C01A				0x32




#if 1
#define NULL ((void *) 0)

#define	U8	unsigned char								/* 8bit unsigned			*/
#define	S8	signed char									/* 8bit signed				*/
#define	U16	unsigned short								/* 16bit unsigned			*/
#define	S16	signed short								/* 16bit signed				*/
#define	U32	unsigned int								/* 32bit unsigned			*/
#define	S32	signed int									/* 16bit signed				*/
#define	DB	double
#else
typedef	unsigned char	U8								/* 8bit unsigned			*/
typedef	unsigned char	S8								/* 8bit signed				*/
typedef	unsigned short	U16								/* 16bit unsigned			*/
typedef	signed short	S16								/* 16bit signed				*/
typedef	unsigned int	U32								/* 32bit unsigned			*/
typedef	signed int		S32								/* 16bit signed				*/
typedef	double			DB
#endif


typedef enum _MT_FE_ON_OFF
{
	MtFe_On = 0
	,MtFe_Off
} MT_FE_ON_OFF;

typedef enum _MT_FE_ERR
{
	MtFeErr_Ok					= 0
	,MtFeErr_Undef				= -1
	,MtFeErr_Uninit				= -2
	,MtFeErr_Param				= -3
	,MtFeErr_NoSupportFunc		= -4
	,MtFeErr_NoSupportTuner		= -5
	,MtFeErr_NoSupportDemod		= -6

	,MtFeErr_UnLock				= -7

	,MtFeErr_I2cErr				= -8


} MT_FE_ERR;




typedef enum _MT_FE_BANDWIDTH
{
	MtFeBandwidth_Undef = 0
	,MtFeBandwidth_6M
	,MtFeBandwidth_7M
	,MtFeBandwidth_8M
} MT_FE_BANDWIDTH;


typedef enum _MT_FE_DMD_TS_OUTPUT_MODE
{
	MtFeDmdTsOutputMode_Undef
	,MtFeDmdTsOutputMode_Serial
	,MtFeDmdTsOutputMode_Parallel
	,MtFeDmdTsOutputMode_Common
} MT_FE_DMD_TS_OUTPUT_MODE;

typedef enum _MT_FE_MOBILE_MODE
{
	MtFeMobileMode_Auto = 0
	,MtFeMobileMode_On
	,MtFeMobileMode_Off
} MT_FE_MOBILE_MODE;

typedef enum _MT_FE_DMD_LOCK_STATE
{
	MtFeDmdLockState_Undef = 0
	,MtFeDmdLockState_Locked
	,MtFeDmdLockState_Unlocked
	,MtFeDmdLockState_Waiting
} MT_FE_DMD_LOCK_STATE;


typedef enum _MT_FE_CARRIER_MODE
{
	MtFeCrrierMode_Undef = 0
	,MtFeCrrierMode_Multicarrier
	,MtFeCrrierMode_SingleCarrier
} MT_FE_CARRIER_MODE;


typedef enum _MT_FE_SPECTRUM_MODE
{
	MtFeSpectrum_Undef = 0
	,MtFeSpectrum_NoInversion
	,MtFeSpectrum_Inversion
} MT_FE_SPECTRUM_MODE;


typedef enum _MT_FE_FRAME_MODE
{
	MtFeFrameMode_Undef = 0
	,MtFeFrameMode_Pn420
	,MtFeFrameMode_Pn945
	,MtFeFrameMode_Pn595
} MT_FE_FRAME_MODE;


typedef enum _MT_FE_INTERLEAVING_DEPTH
{
	MtFeInterleavingDepth_Undef = 0
	,MtFeInterleavingDepth_240
	,MtFeInterleavingDepth_720
} MT_FE_INTERLEAVING_DEPTH;

typedef enum _MT_FE_CONSTELLATION_PATTERN
{
	MtFeConstellationPattern_Undef = 0
	,MtFeConstellationPattern_4Qam
	,MtFeConstellationPattern_4QamNr
	,MtFeConstellationPattern_16Qam
	,MtFeConstellationPattern_32Qam
	,MtFeConstellationPattern_64Qam
	,MtFeConstellationPattern_128Qam
	,MtFeConstellationPattern_256Qam
} MT_FE_CONSTELLATION_PATTERN;

typedef enum _MT_FE_FEC_CODE_RATE
{
	MtFeFecCodeRate_Undef = 0
	,MtFeFecCodeRate_0p4
	,MtFeFecCodeRate_0p6
	,MtFeFecCodeRate_0p8
} MT_FE_FEC_CODE_RATE;


typedef enum _MT_FE_SI_MAP_MODE
{
	MtFeSiMapMode_Undef = 0
	,MtFeSiMapMode_1
	,MtFeSiMapMode_2
} MT_FE_SI_MAP_MODE;


typedef struct _MT_FE_CHAN_INFO
{
	MT_FE_CARRIER_MODE			carrier_mode;
	MT_FE_SPECTRUM_MODE			spectrum_mode;
	MT_FE_FRAME_MODE			frame_mode;
	MT_FE_INTERLEAVING_DEPTH	interleaving_depth;
	MT_FE_CONSTELLATION_PATTERN	constellation_pattern;
	MT_FE_FEC_CODE_RATE			fec_code_rate;
	MT_FE_SI_MAP_MODE			si_map_mode;
} MT_FE_CHAN_INFO;





#if (MT_FE_DEBUG == 1)
	#define mt_fe_print(str)				DBGPrint str
	#define mt_fe_assert(bool,msg)
#else
	#define mt_fe_print(str)
	#define mt_fe_assert(bool,msg)
#endif




#if (MT_FE_TN_SUPPORT_TD1611ALF4 > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_td1611alf4(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_SHARP6093 > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_sharp6093(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_SHARP2093 > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_sharp2093(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_TDTCG0X2D > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_tdtcg0x2d(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_TDQB3_601A > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_tdqb3_601a(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_TDQG4006A > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_tdqg4_006a(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_MxL_5007T > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_MxL5007T(U32 freq);
#endif
#if (MT_FE_TN_SUPPORT_TDAD3_C01A > 0)
extern MT_FE_ERR mt_fe_tn_set_freq_tdad3_c01a(U32 freq);
#endif




//extern MT_FE_ERR mt_fe_tn_set_freq_sharp6093(U32 freq);

MT_FE_ERR mt_fe_dmd_init_dd2k(void);
MT_FE_ERR mt_fe_dmd_connect_dd2k(U32 feq, U32 chan_info_code);
MT_FE_ERR mt_fe_dmd_hard_reset_dd2k(void);
MT_FE_ERR mt_fe_dmd_soft_reset_dd2k(void);
MT_FE_ERR mt_fe_dmd_set_output_mode_dd2k(MT_FE_DMD_TS_OUTPUT_MODE mode);
MT_FE_ERR mt_fe_dmd_set_mobile_mode_dd2k(MT_FE_MOBILE_MODE mode);
MT_FE_ERR mt_fe_dmd_get_lock_state_dd2k(MT_FE_DMD_LOCK_STATE *p_state);
MT_FE_ERR mt_fe_dmd_get_per_dd2k(U16 *p_err_cnt, U16 *p_pkt_cnt);
MT_FE_ERR mt_fe_dmd_get_quality_dd2k(S8 *p_quality);
MT_FE_ERR mt_fe_dmd_get_signal_strength_dd2k(S8 *p_strength);
MT_FE_ERR mt_fe_dmd_get_demod_info_dd2k(MT_FE_CHAN_INFO *p_info);

void _mt_fe_dmd_init(unsigned int chipaddress);
void _mt_tn_dmd_init(unsigned int chipaddress);



#ifdef __cplusplus
}
#endif


#endif /* __MT_FE_DEF_H__ */

