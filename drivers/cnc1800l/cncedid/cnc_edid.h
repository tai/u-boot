#ifndef __EDID_H
#define __EDID_H

typedef enum
{
	CSAUD_HDMI_PCM_DOWN_MIX = 0,
	CSAUD_HDMI_PCM_MULTI_CHANNEL,
	CSAUD_HDMI_DOLBY_DIGITAL,
	CSAUD_HDMI_DOLBY_DIGITAL_PLUS,
	CSAUD_HDMI_HBR,
	CSAUD_HDMI_MAX
}CSAUD_HDMI_MODE;

#define EDID_TEST

#define EDID_MAX_BIN_LENGTH                      1024

#define EDID_BLOCK_LENGTH                        0x80

#define EDID_HEADER_OFFSET                       0
#define EDID_HEADER_LENGTH                       8

#define EDID_VENDOR_PRODUCT_ID_OFFSET            0x8
#define EDID_VENDOR_PRODUCT_ID_LENGTH            10
#define EDID_ID_MFG_NAME_OFFSET                  EDID_VENDOR_PRODUCT_ID_OFFSET
#define EDID_ID_MFG_NAME_LENGTH                  2
#define EDID_ID_PRODUCT_CODE_OFFSET              \
    (EDID_ID_MFG_NAME_OFFSET + EDID_ID_MFG_NAME_LENGTH)
#define EDID_ID_PRODUCT_CODE_LENGTH              2
#define EDID_ID_SERIAL_NUMBER_OFFSET             \
    (EDID_ID_PRODUCT_CODE_OFFSET + EDID_ID_PRODUCT_CODE_LENGTH)
#define EDID_ID_SERIAL_NUMBER_LENGTH             4
#define EDID_WEEK_OF_MFG_OFFSET                  \
    (EDID_ID_SERIAL_NUMBER_OFFSET + EDID_ID_SERIAL_NUMBER_LENGTH)
#define EDID_WEEK_OF_MFG_LENGTH                  1
#define EDID_YEAR_OF_MFG_OFFSET                  \
    (EDID_WEEK_OF_MFG_OFFSET + EDID_WEEK_OF_MFG_LENGTH)
#define EDID_YEAR_OF_MFG_LENGTH                  1

#define EDID_VERSION_OFFSET                      0x12
#define EDID_REVISION_OFFSET                     (EDID_VERSION_OFFSET+1)
#define EDID_VERSION_LENGTH                      2

#define EDID_DISPLAY_PARAMS_OFFSET               0x14
#define EDID_DISPLAY_PARAMS_LENGTH               5
#define EDID_VIDEO_INPUT_DEF_OFFSET              EDID_DISPLAY_PARAMS_OFFSET
#define EDID_VIDEO_INPUT_DEF_SIGNAL_INT_MASK     0x80
#define EDID_VIDEO_INPUT_DEF_SIGNAL_INT_DIGITAL  EDID_VIDEO_INPUT_DEF_SIGNAL_INT_MASK
#define EDID_VIDEO_INPUT_DEF_SIGNAL_INT_ANALOG   0
#define EDID_VIDEO_INPUT_DEF_COLOR_DEPTH_MASK    0x70
#define EDID_VIDEO_INPUT_DEF_COLOR_DEPTH_SHIFT   4
#define EDID_VIDEO_INPUT_DEF_INTERFACE_STD_MASK  0x0f
#define EDID_VIDEO_INPUT_DEF_INTERFACE_STD_SHIFT 0
#define EDID_VIDEO_INPUT_HORIZONTAL_OFFSET       \
    (EDID_VIDEO_INPUT_DEF_OFFSET + 1)
#define EDID_VIDEO_INPUT_VERTICAL_OFFSET         \
    (EDID_VIDEO_INPUT_HORIZONTAL_OFFSET + 1)
#define EDID_VIDEO_INPUT_GAMMA_OFFSET        \
    (EDID_VIDEO_INPUT_VERTICAL_OFFSET + 1)
#define EDID_VIDEO_INPUT_FEATURE_SUPPORT_OFFSET  \
    (EDID_VIDEO_INPUT_GAMMA_OFFSET + 1)
#define EDID_VIDEO_INPUT_STANDBY_MASK            0x80
#define EDID_VIDEO_INPUT_SUSPEND_MASK            0x40
#define EDID_VIDEO_INPUT_ACTIVE_OFF_MASK         0x20
#define EDID_VIDEO_INPUT_COLOR_TYPE_MASK         0x18
#define EDID_VIDEO_INPUT_COLOR_TYPE_SHIFT        3
#define EDID_VIDEO_INPUT_SRGB_MASK               0x04
#define EDID_VIDEO_INPUT_PREFERRED_MASK          0x02
#define EDID_VIDEO_INPUT_CONTINUOUS_MASK         0x01

#define EDID_CHROMATICITY_OFFSET                 0x19
#define EDID_CHROMATICITY_LENGTH                 10
#define EDID_CHROMATICITY_RxLo_OFFSET            EDID_CHROMATICITY_OFFSET
#define EDID_CHROMATICITY_RxLo_MASK              0xc0
#define EDID_CHROMATICITY_RxLo_SHIFT             6
#define EDID_CHROMATICITY_RyLo_OFFSET            EDID_CHROMATICITY_OFFSET
#define EDID_CHROMATICITY_RyLo_MASK              0x30
#define EDID_CHROMATICITY_RyLo_SHIFT             4
#define EDID_CHROMATICITY_GxLo_OFFSET            EDID_CHROMATICITY_OFFSET
#define EDID_CHROMATICITY_GxLo_MASK              0x0c
#define EDID_CHROMATICITY_GxLo_SHIFT             2
#define EDID_CHROMATICITY_GyLo_OFFSET            EDID_CHROMATICITY_OFFSET
#define EDID_CHROMATICITY_GyLo_MASK              0x03
#define EDID_CHROMATICITY_GyLo_SHIFT             0
#define EDID_CHROMATICITY_BxLo_OFFSET            (EDID_CHROMATICITY_OFFSET+1)
#define EDID_CHROMATICITY_BxLo_MASK              0xc0
#define EDID_CHROMATICITY_BxLo_SHIFT             6
#define EDID_CHROMATICITY_ByLo_OFFSET            (EDID_CHROMATICITY_OFFSET+1)
#define EDID_CHROMATICITY_ByLo_MASK              0x30
#define EDID_CHROMATICITY_ByLo_SHIFT             4
#define EDID_CHROMATICITY_WxLo_OFFSET            (EDID_CHROMATICITY_OFFSET+1)
#define EDID_CHROMATICITY_WxLo_MASK              0x0c
#define EDID_CHROMATICITY_WxLo_SHIFT             2
#define EDID_CHROMATICITY_WyLo_OFFSET            (EDID_CHROMATICITY_OFFSET+1)
#define EDID_CHROMATICITY_WyLo_MASK              0x03
#define EDID_CHROMATICITY_WyLo_SHIFT             0

#define EDID_CHROMATICITY_RxHi_OFFSET            (EDID_CHROMATICITY_OFFSET+2)
#define EDID_CHROMATICITY_RyHi_OFFSET            (EDID_CHROMATICITY_OFFSET+3)
#define EDID_CHROMATICITY_GxHi_OFFSET            (EDID_CHROMATICITY_OFFSET+4)
#define EDID_CHROMATICITY_GyHi_OFFSET            (EDID_CHROMATICITY_OFFSET+5)
#define EDID_CHROMATICITY_BxHi_OFFSET            (EDID_CHROMATICITY_OFFSET+6)
#define EDID_CHROMATICITY_ByHi_OFFSET            (EDID_CHROMATICITY_OFFSET+7)
#define EDID_CHROMATICITY_WxHi_OFFSET            (EDID_CHROMATICITY_OFFSET+8)
#define EDID_CHROMATICITY_WyHi_OFFSET            (EDID_CHROMATICITY_OFFSET+9)
#define EDID_CHROMATICITY_HiByte_SHIFT           2
#define EDID_CHROMATICITY_LoByte_MASK            3

#define EDID_CHROMATICITY_Rx_INDEX               0
#define EDID_CHROMATICITY_Ry_INDEX               1
#define EDID_CHROMATICITY_Gx_INDEX               2
#define EDID_CHROMATICITY_Gy_INDEX               3
#define EDID_CHROMATICITY_Bx_INDEX               4
#define EDID_CHROMATICITY_By_INDEX               5
#define EDID_CHROMATICITY_Wx_INDEX               6
#define EDID_CHROMATICITY_Wy_INDEX               7

#define EDID_CHROMATICITY_STORAGE_SIZE           8

#define EDID_ESTABLISHED_TIMINGS_OFFSET          0x23
#define EDID_ESTABLISHED_TIMINGS_LENGTH          3
#define EDID_ESTABLISHED_TIMINGS_I_OFFSET        EDID_ESTABLISHED_TIMINGS_OFFSET
#define EDID_ESTABLISHED_TIMINGS_II_OFFSET       (EDID_ESTABLISHED_TIMINGS_I_OFFSET+1)
#define EDID_ESTABLISHED_TIMINGS_MFG_OFFSET      (EDID_ESTABLISHED_TIMINGS_II_OFFSET+1)

#define EDID_ESTABLISHED_TIMINGS_I_INDEX         0
#define EDID_ESTABLISHED_TIMINGS_II_INDEX        1
#define EDID_ESTABLISHED_TIMINGS_MFG_INDEX       2

#define EDID_STANDARD_TIMINGS_OFFSET             0x26
#define EDID_STANDARD_TIMINGS_LENGTH             16
#define EDID_STANDARD_TIMINGS_LENGTH_EACH        2
#define EDID_STANDARD_TIMINGS_BLOCK_OFFSET(i)    \
    (EDID_STANDARD_TIMINGS_OFFSET + i*EDID_STANDARD_TIMINGS_LENGTH_EACH)
#define EDID_STANDARD_TIMINGS_COUNT              8

#define EDID_STANDARD_TIMINGS_AR_MASK            0xc0
#define EDID_STANDARD_TIMINGS_FIELD_REFRESH_MASK 0x3f

#define EDID_MAIN_18B_OFFSET                     0x36
#define EDID_18B_BLOCK_LENGTH                    18
#define EDID_MAIN_18B_BLOCK_OFFSET(i) (EDID_MAIN_18B_OFFSET+(EDID_18B_BLOCK_LENGTH*i))

#define EDID_DISPLAY_DESC_TAG_NUM_OFFSET         3 // offset from start of 18B block
#define EDID_DISPLAY_DESC_DUMMY                  0x10
#define EDID_DISPLAY_DESC_DISPLAY_SERIAL_NUMBER  0xff
#define EDID_DISPLAY_DESC_ALPHANUM_STRING        0xfe
#define EDID_DISPLAY_DESC_DISPLAY_RANGE_LIMITS   0xfd
#define EDID_DISPLAY_DESC_DISPLAY_PRODUCT_NAME   0xfc
#define EDID_DISPLAY_DESC_COLOR_POINT_DATA       0xfb
#define EDID_DISPLAY_DESC_STD_TIMING_ID          0xfa
#define EDID_DISPLAY_DESC_DCM_DATA               0xf9
#define EDID_DISPLAY_DESC_CVT_3_BYTE_CODES       0xf8
#define EDID_DISPLAY_DESC_EST_TIMINGS_III        0xf7
// shared between serial number, data string and product name
#define EDID_DISPLAY_DESC_STRING_OFFSET          5 // offset from start of 18B block
#define EDID_DISPLAY_DESC_STRING_LENGTH          13 // 13 bytes of data
#define EDID_DISPLAY_DESC_STRING_STORAGE_SIZE    16 // stored in 16 bytes       
#define EDID_DISPLAY_DESC_STD_TIMINGS_OFFSET     5 // offset from start of 18B block
#define EDID_DISPLAY_DESC_STD_TIMINGS_COUNT      6

#define EDID_DISPLAY_RANGE_LIMITS_OFFSET         4
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MASK   3
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MASK   0xc0
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MIN    1
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MAX    2
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MIN    4
#define EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MAX    8
#define EDID_DISPLAY_RANGE_LIMITS_V_MIN_OFFSET   5
#define EDID_DISPLAY_RANGE_LIMITS_V_MAX_OFFSET   6
#define EDID_DISPLAY_RANGE_LIMITS_H_MIN_OFFSET   7
#define EDID_DISPLAY_RANGE_LIMITS_H_MAX_OFFSET   8
#define EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MAX_OFFSET 9
#define EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MULTIPLIER 10
#define EDID_DISPLAY_RANGE_LIMITS_GTF_OFFSET     10

#define EDID_DTD_PIXEL_CLOCK_LO_OFFSET           0
#define EDID_DTD_PIXEL_CLOCK_HI_OFFSET           1
#define EDID_DTD_PIXEL_CLOCK_HI_SHIFT            8
#define EDID_DTD_PIXEL_CLOCK_MULTIPLIER          10000

#define EDID_DTD_H_ACTIVE_LO_OFFSET              2
#define EDID_DTD_H_ACTIVE_HI_OFFSET              4
#define EDID_DTD_H_ACTIVE_HI_MASK                0xf0
#define EDID_DTD_H_ACTIVE_HI_SHIFT               4

#define EDID_DTD_H_BLANK_LO_OFFSET               3
#define EDID_DTD_H_BLANK_HI_OFFSET               4
#define EDID_DTD_H_BLANK_HI_MASK                 0xf
#define EDID_DTD_H_BLANK_HI_SHIFT                8

#define EDID_DTD_V_ACTIVE_LO_OFFSET              5
#define EDID_DTD_V_ACTIVE_HI_OFFSET              7
#define EDID_DTD_V_ACTIVE_HI_MASK                0xf0
#define EDID_DTD_V_ACTIVE_HI_SHIFT               4

#define EDID_DTD_V_BLANK_LO_OFFSET               6
#define EDID_DTD_V_BLANK_HI_OFFSET               7
#define EDID_DTD_V_BLANK_HI_MASK                 0xf
#define EDID_DTD_V_BLANK_HI_SHIFT                8

#define EDID_DTD_H_FRONT_PORCH_LO_OFFSET         8
#define EDID_DTD_H_FRONT_PORCH_HI_OFFSET         11
#define EDID_DTD_H_FRONT_PORCH_HI_MASK           0xc0
#define EDID_DTD_H_FRONT_PORCH_HI_SHIFT          2

#define EDID_DTD_H_SYNC_WIDTH_LO_OFFSET          9
#define EDID_DTD_H_SYNC_WIDTH_HI_OFFSET          11
#define EDID_DTD_H_SYNC_WIDTH_HI_MASK            0x30
#define EDID_DTD_H_SYNC_WIDTH_HI_SHIFT           4

#define EDID_DTD_V_FRONT_PORCH_LO_OFFSET         10
#define EDID_DTD_V_FRONT_PORCH_LO_MASK           0xf0
#define EDID_DTD_V_FRONT_PORCH_LO_SHIFT          4
#define EDID_DTD_V_FRONT_PORCH_HI_OFFSET         11
#define EDID_DTD_V_FRONT_PORCH_HI_MASK           0xc
#define EDID_DTD_V_FRONT_PORCH_HI_SHIFT          2

#define EDID_DTD_V_SYNC_WIDTH_LO_OFFSET          10
#define EDID_DTD_V_SYNC_WIDTH_LO_MASK            0xf
#define EDID_DTD_V_SYNC_WIDTH_LO_SHIFT           0
#define EDID_DTD_V_SYNC_WIDTH_HI_OFFSET          11
#define EDID_DTD_V_SYNC_WIDTH_HI_MASK            0x3
#define EDID_DTD_V_SYNC_WIDTH_HI_SHIFT           4

#define EDID_DTD_H_SIZE_MM_LO_OFFSET             12
#define EDID_DTD_H_SIZE_MM_HI_OFFSET             14
#define EDID_DTD_H_SIZE_MM_HI_MASK               0xf0
#define EDID_DTD_H_SIZE_MM_HI_SHIFT              4

#define EDID_DTD_V_SIZE_MM_LO_OFFSET             13
#define EDID_DTD_V_SIZE_MM_HI_OFFSET             14
#define EDID_DTD_V_SIZE_MM_HI_MASK               0xf
#define EDID_DTD_V_SIZE_MM_HI_SHIFT              8

#define EDID_DTD_BORDER_WIDTH_OFFSET             15
#define EDID_DTD_BORDER_HEIGHT_OFFSET            16

#define EDID_DTD_LO_MASK                         0xff

#define EDID_DTD_FLAGS_OFFSET                    17
#define EDID_DTD_FLAGS_MASK                      0xff
#define EDID_DTD_FLAGS_INTERLACED                0x80
#define EDID_DTD_FLAGS_STEREO_MASK               ((3<<5)|1)
#define EDID_DTD_FLAGS_STEREO_NORMAL0            0 // these two should be treated equally
#define EDID_DTD_FLAGS_STEREO_NORMAL1            1
#define EDID_DTD_FLAGS_STEREO_FS_RIGHT           (1<<5)
#define EDID_DTD_FLAGS_STEREO_FS_LEFT            (2<<5)
#define EDID_DTD_FLAGS_STEREO_2WAY_RIGHT         ((1<<5)|1)
#define EDID_DTD_FLAGS_STEREO_2WAY_LEFT          ((2<<5)|1)
#define EDID_DTD_FLAGS_STEREO_4WAY               (3<<5)
#define EDID_DTD_FLAGS_STEREO_SIDE_BY_SIDE       ((3<<5)|1)
#define EDID_DTD_FLAGS_SYNC_MASK                 0x1e
#define EDID_DTD_FLAGS_SYNC_TYPE_MASK            0x18
#define EDID_DTD_FLAGS_SYNC_SERRATIONS_MASK      4 // for analog and digital cpst sync
#define EDID_DTD_FLAGS_SYNC_SYNC_ON_GREEN_MASK   2 // for analog sync only
#define EDID_DTD_FLAGS_SYNC_VSYNC_POLARITY_MASK  4 // for digital separate sync
#define EDID_DTD_FLAGS_SYNC_HSYNC_POLARITY_MASK  2 // for digital separate sync

#define EDID_EXT_BLOCKS_COUNT                    0x7e
#define EDID_CHECKSUM_OFFSET                     0x7f

#define EDID_EXT_BLOCK_SIZE                      0x80
// count from start of EDID, first EEDID block is block 1
#define EDID_EXT_BLOCK_OFFSET(i)                 (i*EDID_EXT_BLOCK_SIZE)

#define EDID_EXT_BLOCK_TAG_OFFSET                0 // offset from start of block
#define EDID_EXT_BLOCK_TAG_EXPECTED_VALUE        2
#define EDID_EXT_BLOCK_REVISION_OFFSET           1 // offset from start of block
#define EDID_EXT_BLOCK_REVISION_EXPECTED_VALUE   3
#define EDID_EXT_BLOCK_18B_BLOCK_START_OFFSET    2
#define EDID_EXT_BLOCK_FLAGS_OFFSET              3
#define EDID_EXT_BLOCK_FLAGS_UNDERSCAN_MASK      0x80
#define EDID_EXT_BLOCK_FLAGS_AUDIO_MASK          0x40
#define EDID_EXT_BLOCK_FLAGS_YCbCr_444_MASK      0x20
#define EDID_EXT_BLOCK_FLAGS_YCbCr_422_MASK      0x10
#define EDID_EXT_BLOCK_FLAGS_DTD_COUNT_MASK      0x0f
#define EDID_EXT_BLOCK_DATA_BLOCKS_OFFSET        4

#define EDID_EXT_BLOCK_DATA_BLOCK_TAG_MASK       0xe0
#define EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT      5
#define EDID_EXT_BLOCK_DATA_BLOCK_LEN_OFFSET     0
#define EDID_EXT_BLOCK_DATA_BLOCK_LEN_MASK       0x1f

#define EDID_EXT_BLOCK_TYPE_AUDIO                1
#define EDID_EXT_BLOCK_TYPE_VIDEO                2
#define EDID_EXT_BLOCK_TYPE_VENDOR_SPECIFIC      3
#define EDID_EXT_BLOCK_TYPE_SPEAKER              4
#define EDID_EXT_BLOCK_TYPE_VESA_DTC             5
#define EDID_EXT_BLOCK_TYPE_EXTENDED             7
#define EDID_EXT_BLOCK_TYPE_EXTENDED_VIDCAP      0
#define EDID_EXT_BLOCK_TYPE_EXTENDED_COLORIMETRY 5

#define EDID_EXT_BLOCK_TYPE_VIDEO_NATIVE_MASK    0x80
#define EDID_EXT_BLOCK_TYPE_VIDEO_VIC_MASK       0x7f

#define EDID_EXT_BLOCK_SAD_SIZE                  3
#define EDID_EXT_BLOCK_SAD_FORMAT_CODE_MASK      0x78 // four bits
#define EDID_EXT_BLOCK_SAD_FORMAT_CODE_SHIFT     3
#define EDID_EXT_BLOCK_SAD_NUM_CHANNELS_MASK     0x7  // three bits, max num chan - 1

#define EDID_EXT_BLOCK_SAD_FILTER_48khz_max      7

#define EDID_AUDIO_STORAGE_SIZE                  16   // size is arbitrary
#define EDID_AUDIO_FORMAT_RESERVED               0
#define EDID_AUDIO_FORMAT_LPCM                   1
#define EDID_AUDIO_FORMAT_AC3                    2
#define EDID_AUDIO_FORMAT_MPEG1                  3
#define EDID_AUDIO_FORMAT_MP3                    4
#define EDID_AUDIO_FORMAT_MPEG2                  5
#define EDID_AUDIO_FORMAT_AAC                    6
#define EDID_AUDIO_FORMAT_DTS                    7
#define EDID_AUDIO_FORMAT_ATRAC                  8
#define EDID_AUDIO_FORMAT_DSD                    9
#define EDID_AUDIO_FORMAT_E_AC3                  10
#define EDID_AUDIO_FORMAT_DTS_HD                 11
#define EDID_AUDIO_FORMAT_MLP                    12
#define EDID_AUDIO_FORMAT_DST                    13
#define EDID_AUDIO_FORMAT_WMA_PRO                14
#define EDID_AUDIO_FORMAT_EXT                    15

#define EDID_AUDIO_FORMAT_STRS_STORAGE_SIZE      16
#define EDID_AUDIO_FREQ_STRS_STORAGE_SIZE        8

#define EDID_AUDIO_FORMAT_STRS_INITIALIZER {"Reserved", "LPCM", "AC3", "MPEG1", "MP3", "MPEG2", "AAC", "DTS", "ATRAC", "DSD", "E-AC3", "DTS-HD", "MLP", "DST", "WMA Pro", "EXT"}
#define EDID_AUDIO_EXT_FORMAT_STRS_INITIALIZER {"Reserved""HE-AAC", "HE-AACx2", "MPEG Surround")
#define EDID_AUDIO_FREQUENCY_STRS_INITIALIZER {"32", "44.1", "48", "88.2", "96", "176.4", "192"}

#define EDID_AUDIO_FORMAT_LPCM_16_BIT_MASK       1
#define EDID_AUDIO_FORMAT_LPCM_20_BIT_MASK       2
#define EDID_AUDIO_FORMAT_LPCM_24_BIT_MASK       4

#define EDID_SPEAKER_BYTE1_MASK                  0xff
#define EDID_SPEAKER_BYTE2_SHIFT                 8
#define EDID_SPEAKER_BYTE3_VALUE                 0

#define EDID_SPEAKER_FL_FR_MASK                  1
#define EDID_SPEAKER_LFE_MASK                    2
#define EDID_SPEAKER_FC_MASK                     4
#define EDID_SPEAKER_RL_RR_MASK                  8
#define EDID_SPEAKER_RC_MASK                     0x10
#define EDID_SPEAKER_FLC_FRC_MASK                0x20
#define EDID_SPEAKER_RLC_RRC_MASK                0x40
#define EDID_SPEAKER_FLW_FRW_MASK                0x80
#define EDID_SPEAKER_FLH_FRH_MASK                0x100
#define EDID_SPEAKER_TC_MASK                     0x200
#define EDID_SPEAKER_FCH_MASK                    0x400

#define EDID_SPEAKER_STRS_STORAGE_SIZE           11

#define EDID_SPEAKER_STRS_INITIALIZER {"Front Left and Front Right", "Low Frequency Effect", "Front Center", "Rear Left and Rear Right", "Rear Center", "Front Left Center and Front Right Center", "Rear Left Center and Rear Right Center", "Front Left Wide and Front Right Wide", "Front Left High and Front Right High", "Top Center", "Front Center High"}

#define EDID_VENDOR_SPECIFIC_STORAGE_SIZE        32 // arbitrary
#define EDID_VENDOR_SPECIFIC_FILTER_BYTE         5
#define EDID_VENDOR_SPECIFIC_FILTER_DEEP_COLOR   0x87

#define EDID_EXTENDED_TAG_CODE_VIDCAP            0
#define EDID_EXTENDED_TAG_CODE_VENDOR_VDB        1
#define EDID_EXTENDED_TAG_CODE_VESA_VDD_INFO     2
#define EDID_EXTENDED_TAG_CODE_VESA_VDB          3
#define EDID_EXTENDED_TAG_CODE_HDMI_VDB          4
#define EDID_EXTENDED_TAG_CODE_COLORIMETRY       5
#define EDID_EXTENDED_TAG_CODE_CEA_AUDIO         16
#define EDID_EXTENDED_TAG_CODE_VENDOR_ADB        17
#define EDID_EXTENDED_TAG_CODE_HDMI_ADB          18

#define EDID_VIDCAP_S_CE_MASK                    0x03
#define EDID_VIDCAP_S_IT_MASK                    0x0c
#define EDID_VIDCAP_S_IT_SHIFT                   2
#define EDID_VIDCAP_S_PT_MASK                    0x30
#define EDID_VIDCAP_S_PT_SHIFT                   4
#define EDID_VIDCAP_QS_MASK                      0x40
#define EDID_VIDCAP_QY_MASK                      0x80

#define EDID_VIDCAP_STRINGS_STORAGE_SIZE    4
#define EDID_VIDCAP_STRS_INITIALIZER { "Not Supported", "Always Overscanned", "Always Underscanned", "Supports Over- and Underscan"}

#define EDID_COLORIMETRY_BYTE1_MASK              0xff
#define EDID_COLORIMETRY_BYTE2_MASK              0xf00
#define EDID_COLORIMETRY_BYTE2_SHIFT             8

#define EDID_COLORIMETRY_STRS_STORAGE_SIZE       6
#define EDID_COLORIMETRY_STRS_INITIALIZER {"xvYCC601", "xvYCC709", "sYCC602", "AdobeYCC601", "AdobeYCCRGB", "Future Metadata Profile"};

#define EDID_ID_STRING_LENGTH                    64

// -1 if valid, 0 if invalid, bitfield of mismatched checksums otherwise
#define EDID_BIN_INVALID                         0
#define EDID_BIN_VALID                           (-1)

#define EDID_ERROR                               (-1)
#define EDID_SUCCESS                             0

#define EDID_TRUE                                1
#define EDID_FALSE                               0

#define EDID_HEADER_INITIALIZER {0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0}
#define EDID_HEADER_LENGTH                       8

// defs for field cnc_edid_t.edid_valid
#define EDID_VALID_HEADER                        0x00001
#define EDID_VALID_VENDOR_PRODUCT_ID             0x00002
#define EDID_VALID_VERSION                       0x00004
#define EDID_VALID_DISPLAY_PARAMS                0x00008
#define EDID_VALID_CHROMATICITY                  0x00010
#define EDID_VALID_EST_TIMINGS                   0x00020
#define EDID_VALID_STD_TIMINGS                   0x00040
#define EDID_VALID_DISPLAY_SERIAL_NUMBER         0x00080
#define EDID_VALID_DISPLAY_DATA_STR              0x00100
#define EDID_VALID_DISPLAY_PRODUCT_NAME          0x00200
#define EDID_VALID_EXT_BLOCKS_COUNT              0x00400
#define EDID_VALID_DISPLAY_RANGE_LIMITS          0x00800
#define EDID_VALID_EEDID                         0x01000
#define EDID_VALID_SVDS_PRESENT                  0x02000
#define EDID_VALID_AUDIO_FORMATS                 0x04000
#define EDID_VALID_SPEAKER_ALLOC                 0x08000
#define EDID_VALID_VENDOR_SPECIFIC               0x10000
#define EDID_VALID_VIDEO_CAPABILITY              0x20000
#define EDID_VALID_COLORIMETRY                   0x40000
#define EDID_VALID_CHECKSUM                      0x80000

// defs for field cnc_edid_timing_t.internal_flags
#define EDID_TIMING_IFLAGS_LOCATION_EST          0x00001
#define EDID_TIMING_IFLAGS_LOCATION_STD          0x00002
#define EDID_TIMING_IFLAGS_LOCATION_MAIN_18B     0x00004
#define EDID_TIMING_IFLAGS_LOCATION_SVD          0x00008
#define EDID_TIMING_IFLAGS_LOCATION_EXT_18B      0x00010
#define EDID_TIMING_IFLAGS_LOCATION_EXT_18B_STD  0x00020 // a type 0xfa disp desc blk
#define EDID_TIMING_IFLAGS_LOCATION_MASK         0x0003f
#define EDID_TIMING_IFLAGS_AR_VALID              0x00100
#define EDID_TIMING_IFLAGS_PIXEL_CLOCK_VALID     0x00200
#define EDID_TIMING_IFLAGS_V_FREQ_VALID          0x00400
#define EDID_TIMING_IFLAGS_VIC_VALID             0x00800
#define EDID_TIMING_IFLAGS_PREFERRED             0x01000
#define EDID_TIMING_IFLAGS_PIXEL_DOUBLED         0x02000
#define EDID_TIMING_IFLAGS_NATIVE                0x04000
#define EDID_TIMING_IFLAGS_ENTRY_VALID           0x10000

// 17 Est, 8 Std, 4 18B block in the main edid
// 25 timings is the most in any ext edid
// so 17+8+4+25=54 should be enough, use 64 for now
#define EDID_TIMINGS_STORAGE_SIZE       64

typedef struct {
    int vic;
    int h_active;
    int v_active;
    int pixel_clock;
    int v_frequency;
    int ar_numerator;
    int ar_denominator;
    int timing_flags;
    int h_total;
    int v_total;
    int h_blank;
    int v_blank;
    int h_front_porch;
    int v_front_porch;
    int h_sync_width;
    int v_sync_width;
    int h_size_mm;
    int v_size_mm;
    int border_width;
    int border_height;
    int internal_flags;
} cnc_edid_timing_t;

// basically the three bytes of a SAD, masked; byte 0 is separated
typedef struct {
    int format;
    int max_channels;
    int frequencies; // 8 bit vector as defined in tables 45-49 of CEA 861-E
    int extra_data;
} cnc_edid_audio_t;

// used in edid_util.c and edid_ops.c
typedef struct {
    int vic;
    int h_active;
    int v_active;
    char interlaced;
    int v_frequency;
    int ar_numerator;
    int ar_denominator;
    int pixel_doubled;
} cnc_edid_vic_struct_t;

#define EDID_MIN_VIC 1
#define EDID_MAX_VIC 64

typedef struct {
    // user-specified name of this edid
    char id_string[EDID_ID_STRING_LENGTH];

    // binary version of edid and related info
    unsigned char edid_bin[EDID_MAX_BIN_LENGTH];

    // -1 if valid, 0 if invalid, bitfield of mismatched checksums otherwise
    // 0 bit is invalid in bitfield, start with all valid, clear if invalid
    int edid_bin_valid;
    int edid_bin_length;

    // bitfield used to mark which parts of canonical format are valid
    // typically written by parser functions, used by assembler
    int edid_valid;

    // EDID_VENDOR_PRODUCT_ID fields
    unsigned char id_mfg_name[4];
    int id_product_code;
    int id_serial_number;
    int week_of_mfg;
    int year_of_mfg;

    // EDID_VERSION fields
    int version;
    int revision;

    // EDID_DISPLAY_PARAMS fields
    int signal_interface; // analog or digital
    int color_bit_depth;
    int interface_std;
    int video_input_def; // FIXME: analog case not parsed
    int horizontal;
    int vertical;
    int gamma;
    int feature_support;

    // EDID_CHROMATICITY fields
    int chromaticity[EDID_CHROMATICITY_STORAGE_SIZE];

    // EDID_18B fields
    char display_serial_number[EDID_DISPLAY_DESC_STRING_STORAGE_SIZE];
    char display_data_string[EDID_DISPLAY_DESC_STRING_STORAGE_SIZE];
    char display_product_name[EDID_DISPLAY_DESC_STRING_STORAGE_SIZE];

    // Display Range Limits (also from an 18B field)
    int rate_v_min, rate_v_max;
    int rate_h_min, rate_h_max;
    int rate_pixel_clock_max;

    int ext_blocks_count;

    // EEDID fields
    int eedid_flags;
    int speaker_alloc;
    int video_capability;
    int colorimetry;
    cnc_edid_audio_t audio_formats[EDID_AUDIO_STORAGE_SIZE];
    // hurry-up option, needs to be parsed and dealt with better
    unsigned char vendor_specific[EDID_VENDOR_SPECIFIC_STORAGE_SIZE];
    int vendor_specific_length;

    cnc_edid_timing_t timings[EDID_TIMINGS_STORAGE_SIZE];
} cnc_edid_t;

#define EDID_INVALIDATE_BINARY(edid)                              \
    edid->edid_bin_valid = EDID_BIN_INVALID

#define EDID_VALIDATE_BINARY(edid, block)                 \
    do {                                                          \
    if (!(edid->edid_bin_valid & (1<<block))) {   \
        printf("%s: EDID binary is not valid, block %d (%x)\n",    \
           __FUNCTION__, block, edid->edid_bin_valid);                \
        edid_retval = EDID_ERROR;                 \
    } else                                \
        edid_retval = EDID_SUCCESS;               \
    } while (0)

#define EDID_MARK_BAD_CHECKSUM(edid, block)               \
    do {                                                          \
    if (edid->edid_bin_valid == EDID_BIN_VALID)           \
        edid->edid_bin_valid = (1<<block);                \
    else                                                      \
        edid->edid_bin_valid |= (1<<block);               \
    } while (0)

#ifdef  EDID_DEBUG
#define EDID_DEBUG_DUMP(edid, offset, length)                 \
    do {                                                          \
    int i, off=offset;                                        \
    printe("%s:\n\t0x%02x:%2db: ", __FUNCTION__+11, offset, length); \
    for (i=0; i<length; i++)                                  \
        printe("%02x ", edid->edid_bin[off++]);           \
        printe("\n");                             \
    } while (0);
#else
#define EDID_DEBUG_DUMP(edid, offset, length)
#endif

#ifdef  EDID_BIG_ENDIAN

#define EDID_PARSE_SHORT_TO_INT(d, s) \
    d = ( ((*(s))<<8) | (*((s)+1)) )
#define EDID_PARSE_LONG_TO_INT(d, s)                \
    d = ( ((*(s))<<24) | (*((s)+1)<<16) | (*((s)+2)<<8) | (*((s)+3)) )

#error NEED MORE MACROS HERE

#else /* else if EDID_LITTLE_ENDIAN */

// d is dst is an integer variable
// s is a pointer to the edid_bin
#define EDID_PARSE_SHORT_TO_INT(d, s) \
    d = ( ((*((s)+1))<<8) | (*(s)) )
#define EDID_PARSE_LONG_TO_INT(d, s)                \
    d = ( ((*( (s)+3))<<24) | (*((s)+2)<<16) | (*((s)+1)<<8) | (*(s)) )

// d is a pointer to the edid_bin
// s is an integer variable
#define EDID_ASSEMBLE_INT_TO_SHORT(d, s) \
    do {                                 \
    *(d) = s & 0xff;         \
    *(d+1) = (s & 0xff00) >> 8;  \
    } while (0)
#define EDID_ASSEMBLE_INT_TO_LONG(d, s)  \
    do {                                 \
    *(d) = s & 0xff;         \
    *(d+1) = (s & 0xff00) >> 8;  \
    *(d+2) = (s & 0xff0000) >> 16;   \
    *(d+3) = (s & 0xff000000) >> 24; \
    } while (0)

#endif /* EDID_BIG_ENDIAN */

// declarations for functions from cnc_edid_parse.c
int cnc_edid_parse(cnc_edid_t *edid);

// declarations for functions from edid_util.c
void cnc_edid_set_id_string(cnc_edid_t *edid, char *edid_id_string);
int cnc_edid_read_hex(cnc_edid_t *edid);
int cnc_edid_int_to_color_bit_depth(int color_bit_depth);
int cnc_edid_vic_to_edid_timing(cnc_edid_timing_t *timing, int vic);
cnc_edid_timing_t *cnc_edid_get_free_timing_struct(cnc_edid_t *edid);
cnc_edid_audio_t *cnc_edid_get_free_audio_struct(cnc_edid_t *edid);
char *cnc_edid_color_bit_depth_ascii(cnc_edid_t *edid);
char *cnc_edid_interface_std_ascii(cnc_edid_t *edid);
char *cnc_edid_display_color_type_to_ascii(cnc_edid_t *edid);
char *cnc_edid_timing_iflags_location_to_ascii(cnc_edid_timing_t *timing);

#ifdef EDID_FLOATING_POINT_AVAILABLE
float cnc_edid_gamma_to_f(cnc_edid_t *edid);
float cnc_edid_chromaticity_to_f(int chromaticity_val);
int cnc_edid_gamme_from_f(float gamma);
int cnc_edid_chromaticity_from_f(float chromaticity_val);
#else
int cnc_edid_chromaticity_fract(int chromaticity_val);
int cnc_edid_gamma_int(cnc_edid_t *edid);
int cnc_edid_gamma_fract(cnc_edid_t *edid);
#endif

// declarations for functions from edid_ops.c
int cnc_edid_copy(cnc_edid_t *src, cnc_edid_t *dst);
int cnc_edid_clear_bin(cnc_edid_t *edid);
int cnc_edid_clear_canonical(cnc_edid_t *edid);
void cnc_edid_init(cnc_edid_t *edid);
int cnc_edid_filter(cnc_edid_t *edid, int* tvoutmode_supported_res, int* audmode_supported);

// declarations for functions from cnc_edid_filter.c
int cnc_edid_print(cnc_edid_t *edid);
int cnc_edid_print_hex(cnc_edid_t *edid);

// declarations for functions from cnc_edid_assemble.c
int cnc_edid_assemble(cnc_edid_t *edid);
#endif /* __EDID_H */
