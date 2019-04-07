#ifndef _DF_H_
#define _DF_H_

typedef enum
{
	TVOUT_MODE_576I = 0,
	TVOUT_MODE_480I,
	TVOUT_MODE_576P,
	TVOUT_MODE_480P,
	TVOUT_MODE_720P50,
	TVOUT_MODE_720P60,
	TVOUT_MODE_1080I25,
	TVOUT_MODE_1080I30,

	TVOUT_MODE_SECAM,
	TVOUT_MODE_PAL_M,
	TVOUT_MODE_PAL_N,
	TVOUT_MODE_PAL_CN,
	TVOUT_MODE_1080P24,
	TVOUT_MODE_1080P25,
	TVOUT_MODE_1080P30,
	TVOUT_MODE_1080P50,
	TVOUT_MODE_1080P60,

	TVOUT_RGB_640X480_60FPS,
	TVOUT_RGB_800X600_60FPS,
	TVOUT_RGB_1024X768_60FPS,
       TVOUT_RGB_1280X720_60FPS,
       TVOUT_RGB_800X480_60FPS,
       TVOUT_RGB_1440X900_60FPS,
       TVOUT_RGB_1280X1024_60FPS,
       TVOUT_RGB_1360X768_60FPS,
       TVOUT_RGB_1920X1080P30,
       TVOUT_RGB_1920X1080P60,
       TVOUT_RGB_1920X1080I30,
       TVOUT_RGB_1366X768_60FPS,
       TVOUT_MODE_MAX,
} TVOUT_MODE;

typedef enum _DF_VIDEO_FORMAT_
{
	NOT_STANDARD_VIDEO_MODE = -1,
	DISP_YUV_720P_60FPS        =0,
	DISP_YUV_1080I_30FPS       =1,
	DISP_YUV_480P              =2,
	DISP_YUV_NTSC              =3,
	DISP_YUV_720P_50FPS        =4,
	DISP_YUV_1080I_25FPS       =5,
	DISP_YUV_576P              =6,
	DISP_YUV_PAL               =7,
	DISP_YUV_1080P_60FPS       =8,
	DISP_YUV_1080P_50FPS       =9,
	DISP_YUV_1080P_24FPS       =10,
	DISP_YUV_1080P_25FPS       =11,
	DISP_YUV_1080P_30FPS       =12,
	DISP_YUV_1080I_25FPS_AS    =13,
	DISP_RGB_VGA_60FPS         =14,
	DISP_RGB_SVGA_60FPS        =15,
	DISP_RGB_XGA_60FPS         =16,
	DISP_RGB_SXGA_60FPS        =17,
	DISP_RGB_UXGA_60FPS        =18,
	DISP_RGB_720P_60FPS        =19,
	DISP_1440x288P_50_M1       =20,
	DISP_1440x288P_50_M2       =21,
	DISP_1440x288P_50_M3       =22,
	DISP_2880x576I_50          =23,
	DISP_2880x288P_50_M1       =24,
	DISP_2880x288P_50_M2       =25,
	DISP_2880x288P_50_M3       =26,
	DISP_1440x576P_50          =27,
	DISP_1440x240P_60_M1       =28,
	DISP_1440x240P_60_M2       =29,
	DISP_2880x480I_60          =30,
	DISP_2880x240P_60_M1       =31,
	DISP_2880x240P_60_M2       =32,
	DISP_1440x480P_60          =33,
	DISP_RGB_DMT_640x480_60    =34,
	DISP_RGB_DMT_640x480_75    =35,
	DISP_RGB_DMT_800x600_60    =36,
	DISP_RGB_DMT_800x600_75    =37,
	DISP_RGB_DMT_848x480_60    =38,
	DISP_RGB_DMT_1152x864_75   =39,
	DISP_RGB_DMT_1280x768_60   =40,
	DISP_RGB_DMT_1280x960_60   =41,
	DISP_RGB_DMT_1280x1024_60  =42,
	DISP_RGB_DMT_1360x768_60   =43,
	DISP_RGB_DMT_1440x900_60   =44,
	DISP_RGB_CVT_640x480_75    =45,
	DISP_RGB_CVT_800x600_60    =46,
	DISP_RGB_CVT_800x600_75    =47,
	DISP_RGB_CVT_1024x768_60   =48,
	DISP_RGB_CVT_1280x1024_60  =49,
	DISP_RGB_CVT_848x480_60    =50,
	DISP_RGB_CVT_848x480_75    =51,
	DISP_RGB_CVT_1024x576_60   =52,
	DISP_RGB_CVT_1024x576_75   =53,
	DISP_RGB_CVT_1280x720_60   =54,
	DISP_RGB_CVT_1280x720_75   =55,
	DISP_RGB_CVT_640x400_75    =56,
	DISP_RGB_CVT_768x480_60    =57,
	DISP_RGB_CVT_768x480_75    =58,
	DISP_RGB_CVT_1024x640_60   =59,
	DISP_RGB_CVT_1024x640_75   =60,
	DISP_RGB_CVT_1280x800_75   =61,
	DISP_RGB_CVT_800x600_RED   =62,
	DISP_RGB_CVT_1024x768_RED  =63,
	DISP_RGB_CVT_1280x960_RED  =64,
	DISP_RGB_CVT_848x480_RED   =65,
	DISP_RGB_CVT_1024x576_RED  =66,
	DISP_RGB_CVT_1280x720_RED  =67,
	DISP_RGB_CVT_1600x900_RED  =68,
	DISP_RGB_CVT_768x480_RED   =69,
	DISP_RGB_CVT_1024x640_RED  =70,
	DISP_RGB_CVT_1280x800_RED  =71,
	DISP_RGB_CVT_1600x1000_RED =72,
	DISP_RGB_800x480 = 73,
	DISP_RGB_1360x768 = 74,
	DISP_RGB_1366x768= 75,
	DISP_RGB_1920x1080P30 = 76,
	DISP_RGB_1920x1080P60 = 77,
	DISP_RGB_1920x1080I30 = 78,
 	DISP_FMT_MAX,
}DF_VIDEO_FORMAT;

typedef enum
{
        DF_OUTPUT_CVBS_SVIDEO = 0,
        DF_OUTPUT_YPBPR,
        DF_OUTPUT_VGA
} DF_OUTPUT_MODE;

typedef struct _DF_OUTIF_FORMAT_INFO_
{
	int  mIsYUVorRGB;
	int  mWidth;
	int  mHeight;
	int  mFPS;
	int  mFreq;
	int  mIsHD;
	int  mVSyncPol;
	int  mHSyncPol;
	int  mHRes;
	int  mHActive;
	int  mHSyncFP;
	int  mHSyncWidth;
	int  mHSyncBP;
	int  mVRes;
	int  mVActline;
	int  mVSyncFP;
	int  mVSyncWidth;
	int  mVSyncBP;
	int  mVidType;
	int  mVidrptr;
	int  mYCMuxEn;
	int  mPALFmt;
	int  mFmtIdx;
	char *mFmtName;

} df_out_if_timing;

typedef struct DF_OUTPUT_CONFIG_t_{
	int gfx_output[2]; //gfx_layer_id
	int vid_output[3]; //video_layer_id
	DF_OUTPUT_MODE output_mode[2];//outif_id
}DF_OUTPUT_CONFIG_t;

typedef enum _DF_GFX_COLOR_FORMAT_ {
    DF_GFX_CLUT8     = 1,
    DF_GFX_RGB565    = 2,
    DF_GFX_ARGB4444  = 3,
     DF_GFX_ARGB1555  = 5,
    DF_GFX_ARGB8888  = 6,
    DF_GFX_FORMA_MAX,

} DF_GFX_COLOR_FORMAT;

typedef struct _DF_GFX_IMG_ {
    unsigned int PixelWidth;
    unsigned int PixelHeight;
    DF_GFX_COLOR_FORMAT ColorFormat;
    unsigned int StartAddress;
    unsigned int LinePitch;
    //For Color Format A0
    unsigned char  DefaultAlpha;
    unsigned char  DefaultRed;
    unsigned char  DefaultGreen;
    unsigned char  DefaultBlue;
} DF_GFX_IMG;

#endif
