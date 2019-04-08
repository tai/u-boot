/****************************************************************************
  * Copyright (C) 2011 Cavium Inc.
  * All rights reserved
  *
  * [RELEASE HISTORY]                           
  * VERSION  DATE       AUTHOR                  DESCRIPTION
  * 0.1      11-10-10       Jia Ma           		Original
  ****************************************************************************
*/
#include <linux/types.h>
#include <configs/cnc1800l.h>
#include "tve0_1800.h"
#include "tve1_1800.h"
#include "df_reg_def.h"
#include "df_reg_fmt.h"
#include "../../../board/cavium/cnc1800l/power_clock.h"
#include "df.h"

#define df_write(a,b) 	(*(volatile unsigned int*)(a) = (b))
#define df_write16(a,b) 	(*(volatile unsigned short*)(a) = (b))
#define df_write8(a,b) 		(*(volatile unsigned char*)(a) = (b))

#define df_read(a) 		(*(volatile unsigned int*)(a))
#define df_read16(a) 		(*(volatile unsigned short*)(a))
#define df_read8(a) 		(*(volatile unsigned char*)(a))

static int __Is_ColorBar = 0;

extern int printf(const char* fmt, ...);

static unsigned char __tve0_reg_read(unsigned char addr_base)
{
	unsigned char data_rd;
	unsigned int addr ;
	addr = TVE0_REG_BASE +(addr_base);
	data_rd = df_read8(addr);

	return data_rd;
}

static void __tve0_reg_write(unsigned char addr_base, unsigned char data)
{
	unsigned char data_rd;
	unsigned int addr ;
	addr = TVE0_REG_BASE +(addr_base);

	df_write8(addr,data);
	data_rd = df_read8(addr);

	if(data_rd != data)
	{
		printf("TVE0: Reg Write ERROR: addr_base = %02x, wr_data =%08x,rd_data = %08x\n",
				addr_base,data,data_rd);
	}
}

int InitTVE0Raw(TVOUT_MODE DispFormat , int EnableColorBar)
{
	int i = 0;
	int VIDEO_TYPE  = TVOUT_MODE_576I;
	int IsHD = 0;
	int IsProgressive = 0;

	//Map the Display Format Value to TVE0 Format Value
	switch (DispFormat)
	{
		case TVOUT_MODE_SECAM:
			VIDEO_TYPE = TYPE_SECAM;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_N:
			VIDEO_TYPE = TYPE_PAL_N;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_M:
			VIDEO_TYPE = TYPE_PAL_M;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_CN:
			VIDEO_TYPE = TYPE_PAL_CN;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_576I:
			VIDEO_TYPE = TYPE_PAL;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_480I:
			VIDEO_TYPE = TYPE_NTSC;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_1080I30:
			VIDEO_TYPE = TYPE_1080i30;
			IsHD = 1;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_1080I25:
			VIDEO_TYPE = TYPE_1080i;
			IsHD = 1;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_720P60:
			VIDEO_TYPE = TYPE_720p60;
			IsHD = 1;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_720P50:
			VIDEO_TYPE = TYPE_720p50;
			IsHD = 1;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_480P:
			VIDEO_TYPE = TYPE_480p;
			IsHD = 0;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_576P:
			VIDEO_TYPE = TYPE_480p;
			IsHD = 0;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_1080P24:
		case TVOUT_MODE_1080P25:
		case TVOUT_MODE_1080P30:
		case TVOUT_MODE_1080P50:
		case TVOUT_MODE_1080P60:
			return 0;
		default:
			VIDEO_TYPE = TYPE_PAL;
			IsHD = 0;
			IsProgressive = 0;
			break;
	}

	//Hold TVE0 Timing
	__tve0_reg_write(TVE0_REG[0][REG_ADDR],0x1);

	//DEBUG_PRINTF("[TVE0: INFO]probe REV_ID ..%d\n",TVE0_REG[1][REG_ADDR]);

	for (i=2; i<TVE0_REG_NUM; i++)
	{
		//DEBUG_PRINTF("TVE0: ADDR=%03x,WriteDATA=%03x\n",TVE0_REG[i][REG_ADDR],TVE0_REG[i][VIDEO_TYPE]);
		__tve0_reg_write(TVE0_REG[i][REG_ADDR],TVE0_REG[i][VIDEO_TYPE]);
	}

	if (EnableColorBar)
	{
		unsigned char  RegValue = 0;
		RegValue = __tve0_reg_read(0x2);
		RegValue |= (1 << 5);
		__tve0_reg_write(0x2,RegValue);
	}
	__tve0_reg_write(TVE0_REG[0][REG_ADDR],0x0);
	return 0;
}

static unsigned char __tve1_reg_read(unsigned char addr_base)
{
	unsigned char data_rd;
	unsigned int addr ;
	addr = TVE1_REG_BASE +(addr_base);
	data_rd = df_read8(addr);
	return data_rd;
}

static void __tve1_reg_write(unsigned char addr_base, unsigned char data)
{
	unsigned char data_rd;
	unsigned int addr ;
	addr = TVE1_REG_BASE +(addr_base);

	df_write8(addr,data);
	data_rd = df_read8(addr);
	if(data_rd != data){
		printf("TVE1: Reg Write ERROR: addr_base = %02x, wr_data =%08x,rd_data = %08x\n",
				addr_base,data,data_rd);
	}
}

int InitTVE1Raw( TVOUT_MODE DispFormat , int EnableColorBar)
{
	int i = 0;
	int VIDEO_TYPE  = TVOUT_MODE_576I;
	int IsHD = 0;
	int IsProgressive = 0;

	//Map the Display Format Value to TVE1 Format Value
	switch (DispFormat){
		case TVOUT_MODE_SECAM:
			VIDEO_TYPE = TYPE_SECAM;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_N:
			VIDEO_TYPE = TYPE_PAL_N;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_M:
			VIDEO_TYPE = TYPE_PAL_M;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_PAL_CN:
			VIDEO_TYPE = TYPE_PAL_CN;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_576I:
			VIDEO_TYPE = TYPE_PAL;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_480I:
			VIDEO_TYPE = TYPE_NTSC;
			IsHD = 0;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_1080I30:
		case TVOUT_MODE_1080I25:
			VIDEO_TYPE = TYPE_1080i;
			IsHD = 1;
			IsProgressive = 0;
			break;
		case TVOUT_MODE_720P60:
			VIDEO_TYPE = TYPE_720p60;
			IsHD = 1;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_720P50:
			VIDEO_TYPE = TYPE_720p50;
			IsHD = 1;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_480P:
			VIDEO_TYPE = TYPE_480p;
			IsHD = 0;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_576P:
			VIDEO_TYPE = TYPE_480p;
			IsHD = 0;
			IsProgressive = 1;
			break;
		case TVOUT_MODE_1080P24:
		case TVOUT_MODE_1080P25:
		case TVOUT_MODE_1080P30:
			return 0;
		default:
			VIDEO_TYPE = TYPE_PAL;
			IsHD = 0;
			IsProgressive = 0;
			break;
	}
	//printascii("1111111111\n");
	//Hold TVE1 Timing
	__tve1_reg_write(TVE1_REG[0][REG_ADDR],0x1);
	//DEBUG_PRINTF("[TVE1: INFO]probe REV_ID ..%d\n",TVE1_REG[1][REG_ADDR]);
	//printascii("222222222222\n");

	for (i=2; i<TVE1_REG_NUM; i++)
	{
		//DEBUG_PRINTF("TVE1: ADDR=%03x,WriteDATA=%03x\n",TVE1_REG[i][REG_ADDR],TVE1_REG[i][VIDEO_TYPE]);
		__tve1_reg_write(TVE1_REG[i][REG_ADDR],TVE1_REG[i][VIDEO_TYPE]);
	}

	if (EnableColorBar)
	{
		unsigned char RegValue = 0;
		RegValue = __tve1_reg_read(0x2);
		RegValue |= (1 << 5);
		__tve1_reg_write(0x2,RegValue);
	}
	__tve1_reg_write(TVE1_REG[0][REG_ADDR],0x0);

	return 0;
}

static int dfreg_ctrl[7] =
{
	DISP_UPDATE_REG,DISP_UPDATE_RT_REG,DISP_INT_STA_COP,DISP_INT_MSK_COP,DISP_INT_STA_HOST,DISP_INT_MSK_HOST,
	DISP_ARBITOR_QUANTUM
};

static int dfreg_gfx[2][10] =
{
	{DISP_GFX1_CTRL,DISP_GFX1_FORMAT,DISP_GFX1_ALPHA_CTRL,DISP_GFX1_KEY_RED,DISP_GFX1_KEY_BLUE,DISP_GFX1_KEY_GREEN,
		DISP_GFX1_BUF_START,DISP_GFX1_LINE_PITCH,DISP_GFX1_X_POSITON,DISP_GFX1_Y_POSITON,
	},
	{DISP_GFX2_CTRL,DISP_GFX2_FORMAT,DISP_GFX2_ALPHA_CTRL,DISP_GFX2_KEY_RED,DISP_GFX2_KEY_BLUE,DISP_GFX2_KEY_GREEN,
		DISP_GFX2_BUF_START,DISP_GFX2_LINE_PITCH,DISP_GFX2_X_POSITON,DISP_GFX2_Y_POSITON,
	},
};

static int dfreg_video0[41] =
{
	DISP_VIDEO1_CTRL,DISP_VIDEO1_ALPHA_CTRL,DISP_VIDEO1_X_POSITON,DISP_VIDEO1_Y_POSITON,DISP_VIDEO1_SRC_X_CROP,DISP_VIDEO1_SRC_Y_CROP,
	DISP_VIDEO1_LAI_PARAM,DISP_VIDEO1_YV_RATIO,DISP_VIDEO1_YH_RATIO,DISP_VIDEO1_CV_RATIO,DISP_VIDEO1_CH_RATIO,DISP_VIDEO1_Y_NOLINE_D,
	DISP_VIDEO1_C_NOLINE_D,DISP_VIDEO1_Y_NOLINE_K,DISP_VIDEO1_C_NOLINE_K,DISP_VIDEO1_HOR_PHASE,DISP_VIDEO1_VER_PHASE,
	DISP_VIDEO1_CUR_YT_ADDR,DISP_VIDEO1_CUR_CT_ADDR,DISP_VIDEO1_CUR_YB_ADDR,DISP_VIDEO1_CUR_CB_ADDR,DISP_VIDEO1_PRE_YT_ADDR,
	DISP_VIDEO1_PRE_CT_ADDR,DISP_VIDEO1_PRE_YB_ADDR,DISP_VIDEO1_PRE_CB_ADDR,DISP_VIDEO1_POS_YT_ADDR,DISP_VIDEO1_POS_CT_ADDR,
	DISP_VIDEO1_POS_YB_ADDR,DISP_VIDEO1_POS_CB_ADDR,DISP_VIDEO1_PPR_YT_ADDR,DISP_VIDEO1_PPR_CT_ADDR,DISP_VIDEO1_PPR_YB_ADDR,
	DISP_VIDEO1_PPR_CB_ADDR,DISP_VIDEO1_STORE_Y_ADDR,DISP_VIDEO1_STORE_C_ADDR,DISP_VIDEO1_SRC_CTRL,DISP_VIDEO1_COEFF_UP,
	DISP_VIDEO1_COEFF_ADDR,DISP_VIDEO1_F2F_START,DISP_VIDEO1_STA_F2F,DISP_VIDEO1_STA_DISP_NUM
};

static int dfreg_video1[9] =
{
	DISP_VIDEO2_CTRL,DISP_VIDEO2_X_POSITON,DISP_VIDEO2_Y_POSITON,DISP_VIDEO2_YT_ADDR,DISP_VIDEO2_CT_ADDR,DISP_VIDEO2_YB_ADDR,
	DISP_VIDEO2_CB_ADDR,DISP_VIDEO2_SRC_CTRL,DISP_VIDEO2_STA_DISP_NUM
};

static int dfreg_video2[33] =
{
	DISP_VIDEO3_CTRL,DISP_VIDEO3_X_POSITON,DISP_VIDEO3_Y_POSITON,DISP_VIDEO3_SRC_X_CROP,DISP_VIDEO3_SRC_Y_CROP,DISP_VIDEO3_YV_RATIO,
	DISP_VIDEO3_YH_RATIO,DISP_VIDEO3_CV_RATIO,DISP_VIDEO3_CH_RATIO,DISP_VIDEO3_Y_NOLINE_D,DISP_VIDEO3_C_NOLINE_D,
	DISP_VIDEO3_Y_NOLINE_K,DISP_VIDEO3_C_NOLINE_K,DISP_VIDEO3_HOR_PHASE,DISP_VIDEO3_VER_PHASE,DISP_VIDEO3_YT_ADDR,
	DISP_VIDEO3_CT_ADDR,DISP_VIDEO3_YB_ADDR,DISP_VIDEO3_CB_ADDR,DISP_VIDEO3_SRC_CTRL,DISP_VIDEO3_STA_DISP_NUM
};

static int dfreg_comp[5] =
{
	DISP_COMP_BACK_GROUND,DISP_COMP_Z_ORDER,DISP_COMP_COLOR_ADJUST,DISP_COMP_SHARP_Y_CTRL,DISP_COMP_SHARP_C_CTRL
};

static int dfreg_hd2sd[20] =
{
	DISP_HD2SD_CTRL,DISP_HD2SD_DES_SIZE,DISP_HD2SD_YV_RATIO,DISP_HD2SD_YH_RATIO,DISP_HD2SD_CV_RATIO,DISP_HD2SD_CH_RATIO,
	DISP_HD2SD_Y_NOLINE_D,DISP_HD2SD_C_NOLINE_D,DISP_HD2SD_Y_NOLINE_K,DISP_HD2SD_C_NOLINE_K,DISP_HD2SD_HOR_PHASE,
	DISP_HD2SD_VER_PHASE,DISP_HD2SD_YT_ADDR,DISP_HD2SD_CT_ADDR,DISP_HD2SD_YB_ADDR,DISP_HD2SD_CB_ADDR,DISP_HD2SD_COEFF_UP,
	DISP_HD2SD_COEFF_ADDR,DISP_HD2SD_STA_STORE,DISP_HD2SD_STA_STORE_NUM
};

static int dfreg_outif[2][8] =
{
	{DISP_OUTIF1_CTRL,DISP_OUTIF1_X_SIZE,DISP_OUTIF1_Y_SIZE,DISP_OUTIF1_HSYNC,DISP_OUTIF1_VSYNC,
		DISP_OUTIF1_VGA_LEVEL,DISP_OUTIF1_STA_DISP_SIZE,DISP_OUTIF1_STA_LINE
	},
	{DISP_OUTIF2_CTRL,DISP_OUTIF2_X_SIZE,DISP_OUTIF2_Y_SIZE,DISP_OUTIF2_HSYNC,DISP_OUTIF2_VSYNC,
		DISP_OUTIF2_VGA_LEVEL,DISP_OUTIF2_STA_DISP_SIZE,DISP_OUTIF2_STA_LINE
	},
};

static  df_out_if_timing outputFormatInfo[] =
{
	{ 1,1280, 720,60, 74250,    1,  0,         0,       1650,  1280,    110,      40,      220,      750,   720,      5,       5,        20,    0,      0, 0,0, DISP_YUV_720P_60FPS,         "DISP_YUV_720P_60FPS"        },
	{ 1,1920,1080,30, 74250,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,   1080,     2,       5,        15,    1,      0, 0,0, DISP_YUV_1080I_30FPS,        "DISP_YUV_1080I_30FPS"       },
	{ 1,720, 480,60, 27000,    0,  1,         1,        858,   720,     16,      62,       60,      525,   480,      9,       6,        30,    0,      0, 0,0, DISP_YUV_480P,               "DISP_YUV_480P"              },
//	{ 1, 720, 480,60, 27000,    0,  1,         1,        858,   800,     14,      24,       20,      525,   488,      9,       6,        22,    0,      0, 0,0, DISP_YUV_480P,               "DISP_YUV_480P"              },
	{ 1, 720, 480,30, 27000,    0,  1,         1,       1716,  1440,     38,     124,      114,      525,   480,      4,       3,        15,    1,      1, 1,0, DISP_YUV_NTSC,               "DISP_YUV_NTSC"              },
	{ 1,1280, 720,50, 74250,    1,  0,         0,       1980,  1280,    440,      40,      220,      750,   720,      5,       5,        20,    0,      0, 0,0, DISP_YUV_720P_50FPS,         "DISP_YUV_720P_50FPS"        },
	{ 1,1920,1080,25, 74250,    1,  0,         0,       2640,  1920,    528,      44,      148,     1125,   1080,     2,       5,        15,    1,      0, 0,0, DISP_YUV_1080I_25FPS,        "DISP_YUV_1080I_25FPS"       },
	{ 1, 720, 576,50, 27000,    0,  1,         1,        864,   720,     12,      64,       68,      625,   576,      5,       5,        39,    0,      0, 0,0, DISP_YUV_576P,               "DISP_YUV_576P"              },
//	{ 1, 720, 576,50, 27000,    0,  1,         1,        864,   806,     12,      22,       24,      625,   596,      5,       5,        19,    0,      0, 0,0, DISP_YUV_576P,               "DISP_YUV_576P"              },
//	{ 1, 720, 576,50, 27000,    0,  1,         1,        864,   768,     12,      40,       44,      625,   596,      5,       5,        19,    0,      0, 0,0, DISP_YUV_576P,               "DISP_YUV_576P"              },
	{ 1, 720, 576,25, 27000,    0,  1,         1,       1728,  1440,     24,     126,      138,      625,   576,      2,       3,        19,    1,      1, 1,1, DISP_YUV_PAL,                "DISP_YUV_PAL"               },
	{ 1,1920,1080,60,148500,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_YUV_1080P_60FPS,        "DISP_YUV_1080P_60FPS"       },
	{ 1,1920,1080,50,148500,    1,  0,         0,       2640,  1920,    528,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_YUV_1080P_50FPS,        "DISP_YUV_1080P_50FPS"       },
	{ 1,1920,1080,24, 74250,    1,  0,         0,       2750,  1920,    638,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_YUV_1080P_24FPS,        "DISP_YUV_1080P_24FPS"       },
	{ 1,1920,1080,25, 74250,    1,  0,         0,       2640,  1920,    528,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_YUV_1080P_25FPS,        "DISP_YUV_1080P_25FPS"       },
	{ 1,1920,1080,30, 74250,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_YUV_1080P_30FPS,        "DISP_YUV_1080P_30FPS"       },
	{ 1,1920,1080,25, 72000,    1,  1,         0,       2304,  1920,     32,     168,      184,     1250,  1080,     23,       5,        57,    1,      0, 0,0, DISP_YUV_1080I_25FPS_AS,     "DISP_YUV_1080I_50FPS_F1"    },
	{ 0, 640, 480,60, 25175,    0,  1,         1,        800,   640,     16,      96,       48,      525,   480,     10,       2,        33,    0,      0, 0,0, DISP_RGB_VGA_60FPS,          "DISP_RGB_VGA_60FPS"         },
	{ 0, 800, 600,60, 40000,    0,  0,         0,       1056,   800,     40,     128,       88,      628,   600,      1,       4,        23,    0,      0, 0,0, DISP_RGB_SVGA_60FPS,         "DISP_RGB_SVGA_60FPS"        },
	{ 0,1024, 768,60, 65000,    0,  1,         1,       1344,  1024,     24,     136,      160,      806,   768,      3,       6,        29,    0,      0, 0,0, DISP_RGB_XGA_60FPS,          "DISP_RGB_XGA_60FPS"         },
	{ 0,1280,1024,60,108000,    1,  0,         0,       1688,  1280,     48,     112,      248,     1066,  1024,      1,       3,        38,    0,      0, 0,0, DISP_RGB_SXGA_60FPS,         "DISP_RGB_SXGA_60FPS"        },
	{ 0,1600,1200,60,162000,    1,  0,         0,       2160,  1600,     64,     192,      304,     1250,  1200,      1,       3,        46,    0,      0, 0,0, DISP_RGB_UXGA_60FPS,         "DISP_RGB_UXGA_60FPS"        },
	{ 0,1280, 720,60, 74375,    1,  0,         0,       1664,  1280,     56,     136,      192,      746,   720,      1,       3,        22,    0,      0, 0,0, DISP_RGB_720P_60FPS,         "DISP_RGB_720P_60FPS"        },
	{ 1, 360, 288,50, 27000,    0,  1,         1,       1728,  1440,     24,     126,      138,      312,   288,      2,       3,        19,    0,      1, 0,0, DISP_1440x288P_50_M1,        "DISP_1440x288P_50_M1"       },
	{ 1, 360, 288,50, 27000,    0,  1,         1,       1728,  1440,     24,     126,      138,      313,   288,      3,       3,        19,    0,      1, 0,0, DISP_1440x288P_50_M2,        "DISP_1440x288P_50_M2"       },
	{ 1, 360, 288,50, 27000,    0,  1,         1,       1728,  1440,     24,     126,      138,      314,   288,      4,       3,        19,    0,      1, 0,0, DISP_1440x288P_50_M3,        "DISP_1440x288P_50_M3"       },
	{ 1, 720, 576,25, 54000,    0,  1,         1,       3456,  2880,     48,     252,      276,      625,   576,      2,       3,        19,    1,      3, 0,1, DISP_2880x576I_50,           "DISP_2880x576I_50"          },
	{ 1, 720, 288,50, 54000,    0,  1,         1,       3456,  2880,     48,     252,      276,      312,   288,      2,       3,        19,    0,      3, 0,0, DISP_2880x288P_50_M1,        "DISP_2880x288P_50_M1"       },
	{ 1, 720, 288,50, 54000,    0,  1,         1,       3456,  2880,     48,     252,      276,      313,   288,      3,       3,        19,    0,      3, 0,0, DISP_2880x288P_50_M2,        "DISP_2880x288P_50_M2"       },
	{ 1, 720, 288,50, 54000,    0,  1,         1,       3456,  2880,     48,     252,      276,      314,   288,      4,       3,        19,    0,      3, 0,0, DISP_2880x288P_50_M3,        "DISP_2880x288P_50_M3"       },
	{ 1, 720, 576,50, 54000,    0,  1,         1,       1728,  1440,     24,     128,      136,      625,   576,      5,       5,        39,    0,      1, 0,0, DISP_1440x576P_50,           "DISP_1440x576P_50"          },
	{ 1, 720, 240,60, 27000,    0,  1,         1,       1716,  1440,     38,     124,      114,      262,   240,      4,       3,        15,    0,      1, 0,0, DISP_1440x240P_60_M1,        "DISP_1440x240P_60_M1"       },
	{ 1, 720, 240,60, 27000,    0,  1,         1,       1716,  1440,     38,     124,      114,      263,   240,      5,       3,        15,    0,      1, 0,0, DISP_1440x240P_60_M2,        "DISP_1440x240P_60_M2"       },
	{ 1, 720, 480,30, 54000,    0,  1,         1,       3432,  2880,     76,     248,      228,      525,   480,      4,       3,        15,    1,      3, 0,0, DISP_2880x480I_60,           "DISP_2880x480I_60"          },
	{ 1, 720, 240,60, 54000,    0,  1,         1,       3432,  2880,     76,     248,      228,      262,   240,      4,       3,        15,    0,      3, 0,0, DISP_2880x240P_60_M1,        "DISP_2880x240P_60_M1"       },
	{ 1, 720, 240,60, 54000,    0,  1,         1,       3432,  2880,     76,     248,      228,      263,   240,      5,       3,        15,    0,      3, 0,0, DISP_2880x240P_60_M2,        "DISP_2880x240P_60_M2"       },
	{ 1, 720, 480,60, 54000,    0,  1,         1,       1716,  1440,     32,     124,      120,      525,   480,      9,       6,        30,    0,      1, 0,0, DISP_1440x480P_60,           "DISP_1440x480P_60"          },
	{ 0, 640, 480,60, 25175,    0,  1,         1,        800,   640,      8,      96,       40,      525,   480,     10,       2,        33,    0,      0, 0,0, DISP_RGB_DMT_640x480_60,     "DISP_RGB_DMT_640x480_60"    },
	{ 0, 640, 480,75, 31500,    0,  1,         1,        840,   640,     16,      64,      120,      500,   480,      1,       3,        16,    0,      0, 0,0, DISP_RGB_DMT_640x480_75,     "DISP_RGB_DMT_640x480_75"    },
	{ 0, 800, 600,60, 40000,    0,  0,         0,       1056,   800,     40,     128,       88,      628,   600,      1,       4,        23,    0,      0, 0,0, DISP_RGB_DMT_800x600_60,     "DISP_RGB_DMT_800x600_60"    },
	{ 0, 800, 600,75, 49500,    0,  0,         0,       1056,   800,     16,      80,      160,      625,   600,      1,       3,        21,    0,      0, 0,0, DISP_RGB_DMT_800x600_75,     "DISP_RGB_DMT_800x600_75"    },
	{ 0, 848, 480,60, 33750,    0,  0,         0,       1088,   848,     16,     112,      112,      517,   480,      6,       8,        23,    0,      0, 0,0, DISP_RGB_DMT_848x480_60,     "DISP_RGB_DMT_848x480_60"    },
	{ 0,1152, 864,75,108000,    1,  0,         0,       1600,  1152,     64,     128,      256,      900,   864,      1,       3,        32,    0,      0, 0,0, DISP_RGB_DMT_1152x864_75,    "DISP_RGB_DMT_1152x864_75"   },
	{ 0,1280, 768,60, 68250,    1,  1,         0,       1440,  1280,     48,      32,       80,      790,   768,      3,       7,        12,    0,      0, 0,0, DISP_RGB_DMT_1280x768_60,    "DISP_RGB_DMT_1280x768_60"   },
	{ 0,1280, 960,60,108000,    1,  0,         0,       1800,  1280,     96,     112,      312,     1000,   960,      1,       3,        36,    0,      0, 0,0, DISP_RGB_DMT_1280x960_60,    "DISP_RGB_DMT_1280x960_60"   },
	{ 0,1280,1024,60,108000,    1,  0,         0,       1688,  1280,     48,     112,      248,     1066,  1024,      1,       3,        38,    0,      0, 0,0, DISP_RGB_DMT_1280x1024_60,   "DISP_RGB_DMT_1280x1024_60"  },
	{ 0,1360, 768,60, 85500,    1,  0,         0,       1792,  1360,     64,     112,      256,      795,   768,      3,       6,        18,    0,      0, 0,0, DISP_RGB_DMT_1360x768_60,    "DISP_RGB_DMT_1360x768_60"   },
	{ 0,1440, 900,60,106500,    1,  0,         1,       1904,  1440,     80,     152,      232,      934,   900,      3,       6,        25,    0,      0, 0,0, DISP_RGB_DMT_1440x900_60,    "DISP_RGB_DMT_1440x900_60"   },
	{ 0, 640, 480,75, 30625,    0,  0,         1,        816,   640,     24,      64,       88,      502,   480,      1,       3,        18,    0,      0, 0,0, DISP_RGB_CVT_640x480_75,     "DISP_RGB_CVT_640x480_75"    },
	{ 0, 800, 600,60, 38125,    0,  0,         1,       1024,   800,     32,      80,      112,      622,   600,      1,       3,        18,    0,      0, 0,0, DISP_RGB_CVT_800x600_60,     "DISP_RGB_CVT_800x600_60"    },
	{ 0, 800, 600,75, 48875,    0,  0,         1,       1040,   800,     40,      80,      120,      627,   600,      1,       3,        23,    0,      0, 0,0, DISP_RGB_CVT_800x600_75,     "DISP_RGB_CVT_800x600_75"    },
	{ 0,1024, 768,60, 64125,    1,  0,         1,       1344,  1024,     56,     112,      168,      802,   768,      1,       3,        30,    0,      0, 0,0, DISP_RGB_CVT_1024x768_60,    "DISP_RGB_CVT_1024x768_60"   },
	{ 0,1280,1024,60,108875,    1,  0,         1,       1712,  1280,     80,     136,      216,     1060,  1024,      1,       3,        41,    0,      0, 0,0, DISP_RGB_CVT_1280x1024_60,   "DISP_RGB_CVT_1280x1024_60"  },
	{ 0, 848, 480,60, 31500,    0,  0,         1,       1056,   848,     16,      88,      104,      497,   480,      1,       3,        13,    0,      0, 0,0, DISP_RGB_CVT_848x480_60,     "DISP_RGB_CVT_848x480_60"    },
	{ 0, 848, 480,75, 40875,    0,  0,         1,       1088,   848,     32,      88,      120,      502,   480,      1,       3,        18,    0,      0, 0,0, DISP_RGB_CVT_848x480_75,     "DISP_RGB_CVT_848x480_75"    },
	{ 0,1024, 576,60, 46875,    1,  0,         1,       1312,  1024,     40,     104,      144,      597,   576,      1,       3,        17,    0,      0, 0,0, DISP_RGB_CVT_1024x576_60,    "DISP_RGB_CVT_1024x576_60"   },
	{ 0,1024, 576,75, 60625,    1,  0,         1,       1344,  1024,     56,     104,      160,      602,   576,      1,       3,        22,    0,      0, 0,0, DISP_RGB_CVT_1024x576_75,    "DISP_RGB_CVT_1024x576_75"   },
	{ 0,1280, 720,60, 74375,    1,  0,         1,       1664,  1280,     56,     136,      192,      746,   720,      1,       3,        22,    0,      0, 0,0, DISP_RGB_CVT_1280x720_60,    "DISP_RGB_CVT_1280x720_60"   },
	{ 0,1280, 720,75, 95625,    1,  0,         1,       1696,  1280,     72,     136,      208,      752,   720,      1,       3,        28,    0,      0, 0,0, DISP_RGB_CVT_1280x720_75,    "DISP_RGB_CVT_1280x720_75"   },
	{ 0, 640, 400,75, 25000,    0,  0,         1,        800,   640,     16,      64,       80,      418,   400,      1,       3,        14,    0,      0, 0,0, DISP_RGB_CVT_640x400_75,     "DISP_RGB_CVT_640x400_75"    },
	{ 0, 768, 480,60, 28625,    0,  0,         1,        960,   768,     16,      80,       96,      497,   480,      1,       3,        13,    0,      0, 0,0, DISP_RGB_CVT_768x480_60,     "DISP_RGB_CVT_768x480_60"    },
	{ 0, 768, 480,75, 37250,    0,  0,         1,        992,   768,     32,      80,      112,      502,   480,      1,       3,        18,    0,      0, 0,0, DISP_RGB_CVT_768x480_75,     "DISP_RGB_CVT_768x480_75"    },
	{ 0,1024, 640,60, 52750,    1,  0,         1,       1328,  1024,     48,     104,      152,      663,   640,      1,       3,        19,    0,      0, 0,0, DISP_RGB_CVT_1024x640_60,    "DISP_RGB_CVT_1024x640_60"   },
	{ 0,1024, 640,75, 67375,    1,  0,         1,       1344,  1024,     56,     104,      160,      669,   640,      1,       3,        25,    0,      0, 0,0, DISP_RGB_CVT_1024x640_75,    "DISP_RGB_CVT_1024x640_75"   },
	{ 0,1280, 800,75,107250,    1,  0,         1,       1712,  1280,     80,     136,      216,      835,   800,      1,       3,        31,    0,      0, 0,0, DISP_RGB_CVT_1280x800_75,    "DISP_RGB_CVT_1280x800_75"   },
	{ 0, 800, 600,60, 34375,    0,  1,         0,        928,   800,     32,      32,       64,      618,   600,      1,       3,        14,    0,      0, 0,0, DISP_RGB_CVT_800x600_RED,    "DISP_RGB_CVT_800x600_RED"   },
	{ 0,1024, 768,60, 54500,    1,  1,         0,       1152,  1024,     32,      32,       64,      790,   768,      1,       3,        18,    0,      0, 0,0, DISP_RGB_CVT_1024x768_RED,   "DISP_RGB_CVT_1024x768_RED"  },
	{ 0,1280, 960,60, 83375,    1,  1,         0,       1408,  1280,     32,      32,       64,      988,   960,      1,       3,        24,    0,      0, 0,0, DISP_RGB_CVT_1280x960_RED,   "DISP_RGB_CVT_1280x960_RED"  },
	{ 0, 848, 480,60, 28875,    0,  1,         0,        976,   848,     32,      32,       64,      494,   480,      1,       3,        10,    0,      0, 0,0, DISP_RGB_CVT_848x480_RED,    "DISP_RGB_CVT_848x480_RED"   },
	{ 0,1024, 576,60, 40875,    1,  1,         0,       1152,  1024,     32,      32,       64,      593,   576,      1,       3,        13,    0,      0, 0,0, DISP_RGB_CVT_1024x576_RED,   "DISP_RGB_CVT_1024x576_RED"  },
	{ 0,1280, 720,60, 62500,    1,  1,         0,       1408,  1280,     32,      32,       64,      741,   720,      1,       3,        17,    0,      0, 0,0, DISP_RGB_CVT_1280x720_RED,   "DISP_RGB_CVT_1280x720_RED"  },
	{ 0,1600, 900,60, 96000,    1,  1,         0,       1728,  1600,     32,      32,       64,      926,   900,      1,       3,        22,    0,      0, 0,0, DISP_RGB_CVT_1600x900_RED,   "DISP_RGB_CVT_1600x900_RED"  },
	{ 0, 768, 480,60, 26500,    0,  1,         0,        896,   768,     32,      32,       64,      494,   480,      1,       3,        10,    0,      0, 0,0, DISP_RGB_CVT_768x480_RED,    "DISP_RGB_CVT_768x480_RED"   },
	{ 0,1024, 640,60, 45500,    1,  1,         0,       1152,  1024,     32,      32,       64,      659,   640,      1,       3,        15,    0,      0, 0,0, DISP_RGB_CVT_1024x640_RED,   "DISP_RGB_CVT_1024x640_RED"  },
	{ 0,1280, 800,60, 69500,    1,  1,         0,       1408,  1280,     32,      32,       64,      823,   800,      1,       3,        19,    0,      0, 0,0, DISP_RGB_CVT_1280x800_RED,   "DISP_RGB_CVT_1280x800_RED"  },
	{ 0,1600,1000,60,106625,    1,  1,         0,       1728,  1600,     32,      32,       64,     1029,  1000,      1,       3,        25,    0,      0, 0,0, DISP_RGB_CVT_1600x1000_RED,  "DISP_RGB_CVT_1600x1000_RED" },
	{ 0,800,480,60,29500,    0,  1,         0,       992,  800,     24,      72,       96,     500,  480,      3,       7,        10,    0,      0, 0,0, DISP_RGB_800x480,  "DISP_RGB_800x480" },
	{ 0,1360,768,60,84750,    1,  0,         0,       1776,  1360,     72,      136,       208,     798,  768,      3,       5,        22,    0,      0, 0,0, DISP_RGB_1360x768,  "DISP_RGB_1360x768" },
	{ 0,1366,768,60,70200,    1,  1,         1,       1500,  1366,     24,      46,       64,     780,  768,      1,       3,        8,    0,      0, 0,0, DISP_RGB_1366x768,  "DISP_RGB_1366x768" },
	{ 0,1920,1080,60,148500,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_RGB_1920x1080P30,        "DISP_RGB_1920x1080P30"       },
	{ 0,1920,1080,60,148500,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,  1080,      4,       5,        36,    0,      0, 0,0, DISP_RGB_1920x1080P60,        "DISP_RGB_1920x1080P60"       },
	{ 0,1920,1080,30, 74250,    1,  0,         0,       2200,  1920,     88,      44,      148,     1125,   1080,     2,       5,        15,    1,      0, 0,0, DISP_RGB_1920x1080I30,        "DISP_RGB_1920x1080I30"       }
};

df_reg_para dfreg;

DF_OUTPUT_CONFIG_t df_output_config = {
	.gfx_output = {0,0},
	.vid_output = {0,1,0},
	.output_mode = {DF_OUTPUT_YPBPR, DF_OUTPUT_YPBPR},
};

static int tve_2_df[] = {
	DISP_YUV_PAL,
	DISP_YUV_NTSC,
	DISP_YUV_576P,
	DISP_YUV_480P,
	DISP_YUV_720P_50FPS,
	DISP_YUV_720P_60FPS,
	DISP_YUV_1080I_25FPS,
	DISP_YUV_1080I_30FPS,

	DISP_YUV_PAL,
	DISP_YUV_NTSC,
	DISP_YUV_PAL,
	DISP_YUV_PAL,
	DISP_YUV_1080P_24FPS,
	DISP_YUV_1080P_25FPS,
	DISP_YUV_1080P_30FPS,
	DISP_YUV_1080P_50FPS,
	DISP_YUV_1080P_60FPS,

	DISP_RGB_VGA_60FPS,
	DISP_RGB_CVT_800x600_60,
	DISP_RGB_XGA_60FPS,
	DISP_RGB_CVT_1280x720_60,
	DISP_RGB_800x480,
	DISP_RGB_DMT_1440x900_60,
	DISP_RGB_DMT_1280x1024_60,
	DISP_RGB_1360x768,
	DISP_RGB_1920x1080P30,
	DISP_RGB_1920x1080P60,
	DISP_RGB_1920x1080I30,
	DISP_RGB_1366x768,
};

static int tve_2_clock[] = {
	_YUV_PAL,
	_YUV_NTSC,
	_YUV_576P,
	_YUV_480P,
	_YUV_720P_50Hz,
	_YUV_720P_60Hz,
	_YUV_1080I_50Hz,
	_YUV_1080I_60Hz,

	_YUV_PAL,
	_YUV_NTSC,
	_YUV_PAL,
	_YUV_PAL,
	_YUV_1080P_24Hz,
	_YUV_1080P_25Hz,
	_YUV_1080P_30Hz,
	_YUV_1080P_50Hz,
	_YUV_1080P_60Hz,

	_RGB_DMT_640x480_60Hz,
	_RGB_CVT_800x600_60Hz,
	_RGB_DMT_1024x768_60Hz,
	_RGB_CVT_1280x720_60Hz,
	_RGB_CVT_800x480_60Hz,
	_RGB_DMT_1440x900_60Hz,
	_RGB_DMT_1280x1024_60Hz,
	_RGB_DMT_1360x768_60Hz,
	_YUV_1080P_30Hz,
	_YUV_1080P_60Hz,
	_YUV_1080I_60Hz,
	_RGB_CVT_1366x768_60Hz,
};

enum _HDMI_VIDEO_TIMING_
{
	HDMI_NONE_STD_VID_FORMAT       =0,
	HDMI_V640x480p_60Hz            =1,
	HDMI_V720x480p_60Hz_4x3        =2,
	HDMI_V720x480p_60Hz_16x9       =3,
	HDMI_V720x480i_60Hz_4x3        =4,
	HDMI_V720x480i_60Hz_16x9       =5,
	HDMI_V720x576i_50Hz_4x3        =6,
	HDMI_V720x576i_50Hz_16x9       =7,
	HDMI_V720x576p_50Hz_4x3        =8,
	HDMI_V720x576p_50Hz_16x9       =9,
	HDMI_V1280x720p_50Hz           =10,
	HDMI_V1280x720p_60Hz           =11,
	HDMI_V1920x1080i_50Hz          =12,
	HDMI_V1920x1080i_60Hz          =13,
	HDMI_V1920x1080p_50Hz          =14,
	HDMI_V1920x1080p_60Hz          =15,
	HDMI_V1920x1080p_24Hz          =16,

  	HDMI_V800x600p_60Hz		=17,
	HDMI_V1024x768p_60Hz		= 18,
	HDMI_V1280x720p_RGB_60Hz           =19,
	HDMI_V800x480p_60Hz		=20,
	HDMI_V1440x900p_60Hz		=21,
	HDMI_V1280x1024p_60Hz	=22,
	HDMI_V1360x768p_60Hz		=23,
	HDMI_V1920x1080p_RGB_30Hz          =24,
	HDMI_V1920x1080p_RGB_60Hz          =25,
  	HDMI_V1920x1080i_RGB_30Hz          =26,
  	HDMI_V1366x768p_60Hz		=27,
};

enum _HDMI_VIDEO_ASPECT_
{
	HDMI_ASPECT_4x3,
	HDMI_ASPECT_16x9,
};

static int tve_2_hdmi[][2] = {
	{HDMI_V720x576i_50Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x480i_60Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x576p_50Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x480p_60Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V1280x720p_50Hz,HDMI_ASPECT_16x9},
	{HDMI_V1280x720p_60Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080i_50Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080i_60Hz,HDMI_ASPECT_16x9},

	{HDMI_V720x576i_50Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x480i_60Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x576i_50Hz_4x3,HDMI_ASPECT_4x3},
	{HDMI_V720x576i_50Hz_4x3,HDMI_ASPECT_4x3},

	{HDMI_V1920x1080p_24Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080p_50Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080p_60Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080p_50Hz,HDMI_ASPECT_16x9},
	{HDMI_V1920x1080p_60Hz,HDMI_ASPECT_16x9},

	{HDMI_V640x480p_60Hz,HDMI_ASPECT_4x3},

#if 1
	{HDMI_V800x600p_60Hz,HDMI_ASPECT_4x3},
	{HDMI_V1024x768p_60Hz,HDMI_ASPECT_4x3},
	{HDMI_V1280x720p_RGB_60Hz,HDMI_ASPECT_16x9},
	{HDMI_V800x480p_60Hz,-1},
	{HDMI_V1440x900p_60Hz,-1},
	{HDMI_V1280x1024p_60Hz,HDMI_ASPECT_4x3},
       {HDMI_V1360x768p_60Hz,HDMI_ASPECT_16x9},
       {HDMI_V1920x1080p_RGB_30Hz,HDMI_ASPECT_16x9},
       {HDMI_V1920x1080p_RGB_60Hz,HDMI_ASPECT_16x9},
       {HDMI_V1920x1080i_RGB_30Hz,HDMI_ASPECT_16x9},
       {HDMI_V1366x768p_60Hz,HDMI_ASPECT_16x9},
#endif
};

void __df_update_start(void)
{
	df_write(dfreg_ctrl[0], 1);
}

void __df_update_end(void)
{
	df_write(dfreg_ctrl[0], 0);
}

void DFSetOutIFVideoFmt(int OutIFId, DF_VIDEO_FORMAT VideoFmt)
{
	//For Safety Format Switching It's better Disable the output clk First
	df_outif_reg *OutIFReg= NULL;

	OutIFId = OutIFId & 0x1;
	OutIFReg = &dfreg.OutIF[OutIFId];

	OutIFReg->df_outif_control_reg.val = df_read(dfreg_outif[OutIFId][0]);
	OutIFReg->df_outif_x_size_reg.val = df_read(dfreg_outif[OutIFId][1]);
	OutIFReg->df_outif_y_size_reg.val = df_read(dfreg_outif[OutIFId][2]);
	OutIFReg->df_outif_hsync_reg.val = df_read(dfreg_outif[OutIFId][3]);
	OutIFReg->df_outif_vsync_reg.val = df_read(dfreg_outif[OutIFId][4]);

//	OutIFReg->df_outif_vga_level_reg = df_read(dfreg_outif[OutIFId][5]);
	OutIFReg->df_outif_control_reg.bits.iDispEn = 1; // majia add here for debug, need move to other place.
//	OutIFReg->df_outif_control.bits.iColorSpace =!outputFormatInfo[VideoFmt].mIsYUVorRGB;
	OutIFReg->df_outif_control_reg.bits.iIsHD        =outputFormatInfo[VideoFmt].mIsHD;
	OutIFReg->df_outif_control_reg.bits.iPALFmt      =outputFormatInfo[VideoFmt].mPALFmt;
//	OutIFReg->df_outif_control.bits.iRepeatTimes =outputFormatInfo[VideoFmt].mVidrptr;
	OutIFReg->df_outif_control_reg.bits.iIsInterlaced     =outputFormatInfo[VideoFmt].mVidType;
//	OutIFReg->df_outif_control.bits.iYCMux =outputFormatInfo[VideoFmt].mYCMuxEn;
	OutIFReg->df_outif_control_reg.bits.iHSyncPol    =outputFormatInfo[VideoFmt].mHSyncPol;
	OutIFReg->df_outif_control_reg.bits.iVSyncPol    =outputFormatInfo[VideoFmt].mVSyncPol;
	OutIFReg->df_outif_hsync_reg.bits.iHSyncBP    =outputFormatInfo[VideoFmt].mHSyncBP;
	OutIFReg->df_outif_hsync_reg.bits.iHSyncFP     =outputFormatInfo[VideoFmt].mHSyncFP;
	OutIFReg->df_outif_hsync_reg.bits.iHSyncWidth  = outputFormatInfo[VideoFmt].mHSyncWidth;
	OutIFReg->df_outif_x_size_reg.bits.iActPixNum   =outputFormatInfo[VideoFmt].mHActive;
	OutIFReg->df_outif_x_size_reg.bits.iHTotal      =outputFormatInfo[VideoFmt].mHRes;
	OutIFReg->df_outif_vsync_reg.bits.iVSyncFP     =outputFormatInfo[VideoFmt].mVSyncFP;
	OutIFReg->df_outif_vsync_reg.bits.iVSyncWidth  =outputFormatInfo[VideoFmt].mVSyncWidth;
	OutIFReg->df_outif_vsync_reg.bits.iVSyncBP     =outputFormatInfo[VideoFmt].mVSyncBP;
	OutIFReg->df_outif_y_size_reg.bits.iActLineNum  =outputFormatInfo[VideoFmt].mVActline;
	OutIFReg->df_outif_y_size_reg.bits.iVTotal      =outputFormatInfo[VideoFmt].mVRes;
	OutIFReg->df_outif_control_reg.bits.iDownMode    	=1;
	OutIFReg->df_outif_control_reg.bits.iDitherMode      =   0;
	OutIFReg->df_outif_control_reg.bits.iDitherEna       =   1;
	OutIFReg->df_outif_vga_level_reg.bits.iBlackLevel	=	252;
	OutIFReg->df_outif_vga_level_reg.bits.iWhiteLevel    =   800;
#if 0
	OutIFReg->df_outif_control.bits.iChoromaDrop = 1;
	OutIFReg->df_outif_control.bits.iTopOrFrameIntEna = 1;
	OutIFReg->df_outif_control.bits.iBotIntEna = 1;
	OutIFReg->df_outif_control.bits.iDispEna = 1;
	OutIFReg->df_outif_control.bits.iColorModulator = !outputFormatInfo[VideoFmt].mIsYUVorRGB;
#endif

	df_write(dfreg_outif[OutIFId][0], dfreg.OutIF[OutIFId].df_outif_control_reg.val);
	df_write(dfreg_outif[OutIFId][1], dfreg.OutIF[OutIFId].df_outif_x_size_reg.val);
	df_write(dfreg_outif[OutIFId][2], dfreg.OutIF[OutIFId].df_outif_y_size_reg.val);
	df_write(dfreg_outif[OutIFId][3], dfreg.OutIF[OutIFId].df_outif_hsync_reg.val);
	df_write(dfreg_outif[OutIFId][4], dfreg.OutIF[OutIFId].df_outif_vsync_reg.val);

	if(OutIFId == 0){
		df_write(dfreg_outif[OutIFId][5], dfreg.OutIF[OutIFId].df_outif_vga_level_reg.val);
	}

	return;
}

void DFOutIFEna(int OutIFId, int IsEna)
{
	df_outif_reg *OutIFReg= NULL;

	OutIFId = OutIFId & 0x1;
	OutIFReg = &dfreg.OutIF[OutIFId];
	OutIFReg->df_outif_control_reg.val = df_read(dfreg_outif[OutIFId][0]);

	OutIFReg->df_outif_control_reg.bits.iDispEn = IsEna;

	df_write(dfreg_outif[OutIFId][0], OutIFReg->df_outif_control_reg.val);

	return;
}

void DFSetZOrder(int OutIFId, int Gfx1ZOrder, int Gfx2ZOrder, int Video1ZOrder)
{
	df_compositor_reg *Reg= NULL;

	Reg = &dfreg.Comp;
	Reg->df_comp_z_order_reg.val = df_read(dfreg_comp[1]);

	Reg->df_comp_z_order_reg.bits.iGfx1ZOrder   = Gfx1ZOrder;
	Reg->df_comp_z_order_reg.bits.iGfx2ZOrder   = Gfx2ZOrder;
	Reg->df_comp_z_order_reg.bits.iVideo1ZOrder = Video1ZOrder;
//	Reg->df_comp_z_order_reg.bits.iVideo2ZOrder = Video2ZOrder;

	df_write(dfreg_comp[1], Reg->df_comp_z_order_reg.val);
	return;
}

void DFSetBackGroud(int OutIFId, unsigned char BGY, unsigned char BGU, unsigned char BGV)
{
	df_compositor_reg *Reg= NULL;

	Reg = &dfreg.Comp;
	Reg->df_comp_back_ground_reg.val = df_read(dfreg_comp[0]);

	Reg->df_comp_back_ground_reg.bits.iBGY = BGY;
	Reg->df_comp_back_ground_reg.bits.iBGU = BGU;
	Reg->df_comp_back_ground_reg.bits.iBGV = BGV;

	df_write(dfreg_comp[0], Reg->df_comp_back_ground_reg.val);
	return;
}

void DFGfxEna(int GfxId, int IsEna, int IsRGB2YUVEna)
{
	df_gfx_reg *Reg= NULL;

	GfxId   = GfxId & 0x1;
	Reg = &dfreg.Gfx[GfxId];

	Reg->df_gfx_control_reg.val = df_read(dfreg_gfx[GfxId][0]);

	if(DF_OUTPUT_VGA == df_output_config.output_mode[df_output_config.gfx_output[GfxId]]){
		IsRGB2YUVEna = 0;
	}
	else{
		IsRGB2YUVEna = 1;
	}

	if(IsEna)
	{
		Reg->df_gfx_control_reg.bits.iGfxEnable = (1 << df_output_config.gfx_output[GfxId]);
		Reg->df_gfx_control_reg.bits.iRGB2YUVConvertEna = IsRGB2YUVEna;
	}
	else
	{
		Reg->df_gfx_control_reg.bits.iGfxEnable = 0;
	}

	printf("layer_output.gfx_output[%d] = %d\n",GfxId,df_output_config.gfx_output[GfxId]);
	printf("Reg->df_gfx_control_reg.bits.iGfxEna = %d\n",Reg->df_gfx_control_reg.bits.iGfxEnable);

	df_write(dfreg_gfx[GfxId][0], Reg->df_gfx_control_reg.val);

	return;
}

void DFGfxSetAlpha(int GfxId, unsigned short DefaultAlpha, unsigned short ARGB1555Alpha0, unsigned short ARGB1555Alpha1)
{
	df_gfx_reg *Reg= NULL;

	GfxId   = GfxId & 0x1;
	Reg = &dfreg.Gfx[GfxId];

	Reg->df_gfx_alpha_control_reg.val = df_read(dfreg_gfx[GfxId][2]);

	Reg->df_gfx_alpha_control_reg.bits.iDefaultAlpha   = DefaultAlpha & 0x3ff;
	Reg->df_gfx_alpha_control_reg.bits.iArgb1555Alpha0 = ARGB1555Alpha0 & 0x3ff;
	Reg->df_gfx_alpha_control_reg.bits.iArgb1555Alpha1 = ARGB1555Alpha1 & 0x3ff;

	df_write(dfreg_gfx[GfxId][2], Reg->df_gfx_alpha_control_reg.val);
	return;
}

void DFGfxGetAlpha(int GfxId, unsigned short *DefaultAlpha, unsigned short *ARGB1555Alpha0, unsigned short *ARGB1555Alpha1)
{
	df_gfx_reg *Reg= NULL;

	GfxId   = GfxId & 0x1;
	Reg = &dfreg.Gfx[GfxId];


	Reg->df_gfx_alpha_control_reg.val = df_read(dfreg_gfx[GfxId][2]);

	*DefaultAlpha = Reg->df_gfx_alpha_control_reg.bits.iDefaultAlpha;
	*ARGB1555Alpha0 = Reg->df_gfx_alpha_control_reg.bits.iArgb1555Alpha0;
	*ARGB1555Alpha1 = Reg->df_gfx_alpha_control_reg.bits.iArgb1555Alpha1;

	return;
}

void DFGfxSetColorKey(int GfxId, unsigned char KeyRMin, unsigned char KeyRMax, unsigned char KeyGMin, unsigned char KeyGMax, unsigned char KeyBMin, unsigned char KeyBMax)
{
	df_gfx_reg *Reg= NULL;

	GfxId   = GfxId & 0x1;
	Reg = &dfreg.Gfx[GfxId];

	Reg->df_gfx_key_red_reg.val = df_read(dfreg_gfx[GfxId][3]);
	Reg->df_gfx_key_blue_reg.val = df_read(dfreg_gfx[GfxId][4]);
	Reg->df_gfx_key_green_reg.val = df_read(dfreg_gfx[GfxId][5]);

	Reg->df_gfx_key_red_reg.bits.iKeyRedMin   = KeyRMin;
	Reg->df_gfx_key_red_reg.bits.iKeyRedMax   = KeyRMax;
	Reg->df_gfx_key_blue_reg.bits.iKeyBlueMin  = KeyBMin;
	Reg->df_gfx_key_blue_reg.bits.iKeyBlueMax  = KeyBMax;
	Reg->df_gfx_key_green_reg.bits.iKeyGreenMin = KeyGMin;
	Reg->df_gfx_key_green_reg.bits.iKeyGreenMax = KeyGMax;

	df_write(dfreg_gfx[GfxId][3], Reg->df_gfx_key_red_reg.val);
	df_write(dfreg_gfx[GfxId][4], Reg->df_gfx_key_blue_reg.val);
	df_write(dfreg_gfx[GfxId][5], Reg->df_gfx_key_green_reg.val);

	return;
}

void DFGfxColorKeyEna(int GfxId,int IsEna)
{
	df_gfx_reg*Reg = NULL;

	GfxId = GfxId & 0x1;
	Reg = &dfreg.Gfx[GfxId];
	Reg->df_gfx_control_reg.val = df_read(dfreg_gfx[GfxId][0]);

	Reg->df_gfx_control_reg.bits.iColorKeyEnable = IsEna;

	df_write(dfreg_gfx[GfxId][0], Reg->df_gfx_control_reg.val);

	return;
}

//Analog Video
typedef enum _DAC1_MODE_
{
	_dac1_tve0_ypbpr =0,
	_dac1_tve1_ypbpr =1,
	_dac1_disp0_rgb  =2,
	_dac1_tve1_cvbs  =3,
	_dac1_disp1_rgb  =4, 
}DAC1_MODE;
#define REG_PINMUX_I2C		(CNC1800L_PINMUX_BASE + (0x004>>1))
#define REG_PINMUX_DISP	(CNC1800L_PINMUX_BASE + (0x014>>1))
void pinmux_set_dac1mode(DAC1_MODE Mode)
{
	unsigned short reg_val = 0;

	reg_val = df_read16(REG_PINMUX_I2C);

	switch(Mode)
	{
		case _dac1_tve0_ypbpr:
//			pinmux_base[REG_PINMUX_DISP>>1] &= (0<<8);
			df_write16(REG_PINMUX_DISP, (0<<8));
			break;
		case _dac1_tve1_ypbpr:
//			pinmux_base[REG_PINMUX_DISP>>1] = (5<<8);
			df_write16(REG_PINMUX_DISP, (5<<8));
			break;
		case _dac1_disp0_rgb:
//			pinmux_base[REG_PINMUX_DISP>>1] = (1<<8);
//			pinmux_base[REG_PINMUX_I2C>>1] |= (1<<1);
//			pinmux_base[REG_PINMUX_I2C>>1] |= (1<<2);
//			pinmux_base[REG_PINMUX_I2C>>1] |= (1<<3);
			df_write16(REG_PINMUX_DISP, (1<<8));
			df_write16(REG_PINMUX_I2C, reg_val |0xe);
			break;
		case _dac1_disp1_rgb:
//			pinmux_base[REG_PINMUX_DISP>>1] = (6<<8);
//			pinmux_base[REG_PINMUX_I2C>>1] |= (1<<1);
//			pinmux_base[REG_PINMUX_I2C>>1] |= (1<<2);
//			pinmux_base[REG_PINMUX_I2C>>1] &= ~(1<<3);
			df_write16(REG_PINMUX_DISP, (6<<8));
			df_write16(REG_PINMUX_I2C, reg_val |0x6);
			break;
		case _dac1_tve1_cvbs:
//			pinmux_base[REG_PINMUX_DISP>>1] = (4<<8);
			df_write16(REG_PINMUX_DISP, reg_val & (4<<8));
			break;
	}

	return;
}

extern unsigned int GFXLINEPITCHSIZE;
extern unsigned int GFX2BASEADDRESS;//not use so far
extern unsigned int screen_w,screen_h,picture_color_mode;
extern int HDMIEnableHDCP(unsigned int);

unsigned int gfxid = 0;
static int	CS_LoadDisplay (DF_GFX_IMG* ColorImg,int IsGfxScalerEna,int IsFetchOneField, int IsFetchTopOnly,int IsDispInterlaced)
{
	dfreg.Gfx[gfxid].df_gfx_control_reg.val = df_read(dfreg_gfx[gfxid][0]);
	dfreg.Gfx[gfxid].df_gfx_format_reg.val = df_read(dfreg_gfx[gfxid][1]);
	dfreg.Gfx[gfxid].iStartAddr = df_read(dfreg_gfx[gfxid][6]);
	dfreg.Gfx[gfxid].iLinePitch= df_read(dfreg_gfx[gfxid][7]);
	dfreg.Gfx[gfxid].df_gfx_x_position_reg.val = df_read(dfreg_gfx[gfxid][8]);
	dfreg.Gfx[gfxid].df_gfx_y_position_reg.val = df_read(dfreg_gfx[gfxid][9]);

	printf("ColorImg->LinePitch 0x%x,ColorImg->StartAddress 0x%x,ColorImg->PixelHeight %d,ColorImg->PixelWidth %d\n\n",
		ColorImg->LinePitch,ColorImg->StartAddress,ColorImg->PixelHeight,ColorImg->PixelWidth);

	printf("ColorImg->ColorFormat %d\n\n",ColorImg->ColorFormat);

	dfreg.Gfx[gfxid].df_gfx_control_reg.bits.iFetchType = IsFetchOneField ? (IsFetchTopOnly ? 1:2): 0;
	dfreg.Gfx[gfxid].df_gfx_format_reg.bits.iColorFormat = ColorImg->ColorFormat;
	dfreg.Gfx[gfxid].iStartAddr = ColorImg->StartAddress;
	dfreg.Gfx[gfxid].iLinePitch = ColorImg->LinePitch;
	dfreg.Gfx[gfxid].df_gfx_x_position_reg.bits.iXStart  = 0;
	dfreg.Gfx[gfxid].df_gfx_x_position_reg.bits.iXEnd   = screen_w - 1;
	dfreg.Gfx[gfxid].df_gfx_y_position_reg.bits.iYStart  = 0;
	dfreg.Gfx[gfxid].df_gfx_y_position_reg.bits.iYEnd   = (screen_h)/ (IsDispInterlaced ? 2:1);

	__df_update_start( );

	df_write(dfreg_gfx[gfxid][0], dfreg.Gfx[gfxid].df_gfx_control_reg.val);
	df_write(dfreg_gfx[gfxid][1], dfreg.Gfx[gfxid].df_gfx_format_reg.val);
	df_write(dfreg_gfx[gfxid][6], dfreg.Gfx[gfxid].iStartAddr);
	df_write(dfreg_gfx[gfxid][7], dfreg.Gfx[gfxid].iLinePitch);
	df_write(dfreg_gfx[gfxid][8], dfreg.Gfx[gfxid].df_gfx_x_position_reg.val);
	df_write(dfreg_gfx[gfxid][9], dfreg.Gfx[gfxid].df_gfx_y_position_reg.val);

	__df_update_end();

	udelay(100000);

	__df_update_start();

	DFGfxSetAlpha(gfxid, 0x3ff, 0, 0);
	DFGfxSetColorKey(gfxid, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	DFGfxColorKeyEna(gfxid, 1);
	DFGfxEna(gfxid, 1, 0);

	__df_update_end( );

	printf("Display configurate complete!\n\n\n");

#if 0
	printf("DISP_GFX1_CTRL : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_control_reg.val);
	printf("DISP_GFX1_FORMAT : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_format_reg.val);
	printf("DISP_GFX1_BUF_START : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_buf_start_addr_reg.val);
	printf("DISP_GFX1_LINE_PITCH : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_line_pitch_reg.val);
	printf("DISP_GFX1_X_POSITON : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_x_position_reg.val);
	printf("DISP_GFX1_Y_POSITON : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_y_position_reg.val);
	printf("DISP_GFX1_SCL_X_POSITON : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_scl_x_position_reg.val);
	printf("DISP_GFX1_ALPHA_CTRL : 0x%x\n",dfreg.Gfx[gfxid].df_gfx_alpha_control_reg.val);
#endif
    return 0;
}

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE-1))

#define SECURITY_CFG_ADDR         0xFFFFF010
#define SECURITY_CFG_UID_OFFSET   0x4

#define security_cfg_read(a)    (*(volatile unsigned int*)(a))

// Determine whether the chip has HDCP 1.x security.
//
// This is kind of a kludge, but all chips that have a
// Unique ID programmed also have HDCP 1.x keys.
static unsigned int HDMISecureChip(void)
{
	unsigned int result = 0;

	result = security_cfg_read(SECURITY_CFG_ADDR+SECURITY_CFG_UID_OFFSET);

	return (result > 0);
}


static int initstatus = 0;
static int per_mode = -1;
int gfx_init(TVOUT_MODE mode, int pic_w, int pic_h, int dst_x, int dst_y)
{
	DF_GFX_IMG ColorImg;

	int IsGfxScalerEna;
	int IsFetchOneField;
	int IsFetchTopOnly;
	int IsDispInterlaced;
	int outifid_new = 0;

	if (initstatus) {	/* already initialized */
		printf("DF and TVE have been already initialized\n");
//		return -1;
	}
	printf("tvmode = %d\n",mode);
	printf("GFXLINEPITCHSIZE = %d,screen_w %d, screen_h %d,picture_color_mode %d\n ",GFXLINEPITCHSIZE,screen_w,screen_h,picture_color_mode);

	ColorImg.StartAddress = GFXBASEADDRESS;
	ColorImg.PixelWidth = pic_w;
	ColorImg.PixelHeight = pic_h;
	ColorImg.ColorFormat = picture_color_mode;
	ColorImg.DefaultAlpha = 0xff;
	ColorImg.LinePitch = GFXLINEPITCHSIZE;

	switch(mode){
		case TVOUT_MODE_576I:
		case TVOUT_MODE_480I:
		case TVOUT_MODE_1080I25:
		case TVOUT_MODE_1080I30:
		case TVOUT_MODE_SECAM:
		case TVOUT_MODE_PAL_M:
		case TVOUT_MODE_PAL_N:
		case TVOUT_MODE_PAL_CN:
			IsDispInterlaced = 1;
			break;
		default :
			IsDispInterlaced = 0;
			break;
	}

	IsGfxScalerEna = 0;
	IsFetchOneField = 0;
	IsFetchTopOnly = 0;

	printf("pic_w = %d,pic_h = %d,dst_x = %d,dst_y = %d\n",pic_w,pic_h,dst_x,dst_y);

//	clock_dispset_dac0mode(_clock_wake);
//	clock_dispset_dac1mode(_clock_wake);

	switch (mode){
		case TVOUT_MODE_SECAM:
		case TVOUT_MODE_PAL_N:
		case TVOUT_MODE_PAL_M:
		case TVOUT_MODE_PAL_CN:
		case TVOUT_MODE_576I:
		case TVOUT_MODE_576P:
		case TVOUT_MODE_480I:
		case TVOUT_MODE_480P:
		case TVOUT_MODE_720P50:
		case TVOUT_MODE_720P60:
		case TVOUT_MODE_1080I25:
		case TVOUT_MODE_1080I30:
		case TVOUT_MODE_1080P24:
		case TVOUT_MODE_1080P25:
		case TVOUT_MODE_1080P30:
		case TVOUT_MODE_1080P50:
		case TVOUT_MODE_1080P60:
			pinmux_set_dac1mode(_dac1_tve0_ypbpr);
			df_output_config.output_mode[outifid_new] = DF_OUTPUT_YPBPR;
			break;
		case TVOUT_RGB_640X480_60FPS:
		case TVOUT_RGB_800X600_60FPS:
			pinmux_set_dac1mode(_dac1_disp0_rgb);
			df_output_config.output_mode[outifid_new] = DF_OUTPUT_VGA;
			break;
		case TVOUT_RGB_1024X768_60FPS:
		case TVOUT_RGB_1280X720_60FPS:
			pinmux_set_dac1mode(_dac1_disp0_rgb);
			df_output_config.output_mode[outifid_new] = DF_OUTPUT_VGA;
			break;
		break;
			default:
		break;
	}

	if(per_mode !=mode){
		clock_dispset_clockmode0(outifid_new, tve_2_clock[mode]);
		clock_hdmiset_clockmode( tve_2_clock[mode], _1x_disp_clk);
		udelay(100);
	}

//	InitTVE0Raw(mode, __Is_ColorBar);
//	InitTVE1Raw(mode, __Is_ColorBar);
	udelay(100);

	if(per_mode !=mode){
		__df_update_start();
		DFSetOutIFVideoFmt(outifid_new, tve_2_df[mode]);
		__df_update_end();
	}

	CS_LoadDisplay (&ColorImg, IsGfxScalerEna,IsFetchOneField, IsFetchTopOnly,IsDispInterlaced);
//	hd2sd_and_sdoutput(mode);

//	gpio_hw_set_direct(1, 1);
//	gpio_hw_write(1, 1);

	per_mode = mode;
	initstatus = 1;
	return 0;
}

