/*
 * Copyright (c) 2003-2011, Cavium Networks. All rights reserved.
 *
 * This Software is the property of Cavium Networks.  The Software and all
 * accompanying documentation are copyrighted.  The Software made available
 * here constitutes the proprietary information of Cavium Networks.  You
 * agree to take reasonable steps to prevent the disclosure, unauthorized use
 * or unauthorized distribution of the Software.  You shall use this Software
 * solely with Cavium hardware.
 *
 * Except as expressly permitted in a separate Software License Agreement
 * between You and Cavium Networks, you shall not modify, decompile,
 * disassemble, extract, or otherwise reverse engineer this Software.  You
 * shall not make any copy of the Software or its accompanying documentation,
 * except for copying incident to the ordinary and intended use of the
 * Software and the Underlying Program and except for the making of a single
 * archival copy.
 *
 * This Software, including technical data, may be subject to U.S.  export
 * control laws, including the U.S.  Export Administration Act and its
 * associated regulations, and may be subject to export or import regulations
 * in other countries.  You warrant that You will comply strictly in all
 * respects with all such regulations and acknowledge that you have the
 * responsibility to obtain licenses to export, re-export or import the
 * Software.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
 * REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
 * DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
 * PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
 * POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
 * OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 *
 */
#include "cnc_edid.h"
#include "../df/df.h"
#ifdef EDID_TEST
int cnc_edid_bin_cmp(cnc_edid_t *edid1, cnc_edid_t *edid2) {
    int i, retval=EDID_SUCCESS;
    for(i=0; i<EDID_MAX_BIN_LENGTH; i++) {
        if(edid1->edid_bin[i] == edid2->edid_bin[i]) {
            continue;
        }
        if(i==0x23 && ((edid1->edid_bin[i] | edid2->edid_bin[i]) == 0x20)) {
            printf("%s: warning, original EDID didn't include 640x480 in EST timings\n", __FUNCTION__);
            continue;
        }
        printf("%s: difference found at byte 0x%x:%02x vs %02x\n", __FUNCTION__, i, edid1->edid_bin[i], edid2->edid_bin[i]);
        retval = EDID_ERROR;
        if(i<0x7f) {
            i = 0x7e;
        }
        else if(i<0xff) {
            i = 0xfe;
        }
    }
    if(retval) {
        cnc_edid_print_hex(edid1);
        printf("\n");
        cnc_edid_print_hex(edid2);
    }
    return retval;
}
#endif

int cnc_edid_copy(cnc_edid_t *src, cnc_edid_t *dst) {
    memcpy(dst, src, sizeof(cnc_edid_t));
    return EDID_SUCCESS;
}

int cnc_edid_clear_bin(cnc_edid_t *edid) {
  //  bzero(edid->edid_bin, EDID_MAX_BIN_LENGTH * sizeof(unsigned char));
    memset(edid->edid_bin, 0, EDID_MAX_BIN_LENGTH * sizeof(unsigned char));
    EDID_INVALIDATE_BINARY(edid);
    return EDID_SUCCESS;
}

int cnc_edid_clear_canonical(cnc_edid_t *edid) {
    int valid, length, edid_size, edid_bin_size;
    if(edid->edid_bin_valid != EDID_BIN_VALID) {
        printf("%s: warning: edid bin not valid, why clear only canonical?\n", __FUNCTION__);
    }
    valid = edid->edid_bin_valid;
    length = edid->edid_bin_length;
    edid_size = sizeof(cnc_edid_t);
    edid_bin_size = EDID_MAX_BIN_LENGTH * sizeof(unsigned char);
//    bzero((void*)(((unsigned int)edid)+edid_bin_size), edid_size-edid_bin_size);
    memset((void*)(((unsigned int)edid)+edid_bin_size), 0, edid_size-edid_bin_size);
    edid->edid_bin_valid = valid;
    edid->edid_bin_length = length;
    return EDID_SUCCESS;
}

void cnc_edid_init(cnc_edid_t *edid) {
//    bzero(edid, sizeof(cnc_edid_t));
    memset(edid, 0, sizeof(cnc_edid_t));
}

//#define NETHD_SUPPORT_TIMINGS_COUNT 20
// convenient struct to use for these compares, though the vic and AR info is not used
// sorted by ascending h_active!!!!
/*
static cnc_edid_vic_struct_t supported_res[NETHD_SUPPORT_TIMINGS_COUNT]= {
    { 1,   640,480,'p',       60,  4, 3, 0},
    { 2,   720,480,'p',       60,  4, 3, 0},
    { 6,   720,480,'i',       60,  4, 3, 1},
    {17,   720,576,'p',       50,  4, 3, 0},
    {21,   720,576,'i',       50,  4, 3, 1},
    { 0,   800,600,'p',       60,  4, 3, 0},
    { 0,  1024,768,'p',       60,  4, 3, 0},
    { 0,  1280,1024,'p',      60,  4, 3, 0},
    { 0,  1280,768,'p',       60,  5, 3, 0},
    { 0,  1280,960,'p',       60,  5, 3, 0},
    { 0,  1600,1200,'p',      60,  4, 3, 0},

    {60,  1280,720,'p',       24, 16, 9, 0},
    {62,  1280,720,'p',       30, 16, 9, 0},

    {32,  1920,1080,'p',      24, 16, 9, 0},
    {34,  1920,1080,'p',      30, 16, 9, 0},

    { 5,  1920,1080,'i',      60, 16, 9, 0}, // svds go through vic table in edid_util.c
    {19,  1280,720,'p',       50, 16, 9, 0},
    {31,  1920,1080,'p',      50, 16, 9, 0},
    { 4,  1280,720,'p',       60, 16, 9, 0},
    {16,  1920,1080,'p',      60, 16, 9, 0},
#if 0
    {39,  1920,1080,'i',      50, 16, 9, 0},
    { 0,  1920,1200,'p',      50, 16,10, 0},
    { 5,  1920,540,'i',       60, 16, 9, 0}, // dtds have field v_active
    {39,  1920,540,'i',       50, 16, 9, 0},
#endif
};
static int tvoutmode[NETHD_SUPPORT_TIMINGS_COUNT]= {
    TVOUT_RGB_640X480_60FPS, 
    TVOUT_MODE_480P, 
    TVOUT_MODE_480I, 
    TVOUT_MODE_576P, 
    0, 
    TVOUT_RGB_800X600_60FPS, 
    TVOUT_RGB_1024X768_60FPS,
    TVOUT_RGB_1280X1024_60FPS,  
    0, 
    0, 
    0, 
    0, 
    0,
    TVOUT_MODE_1080P24, 
    TVOUT_MODE_1080P30, 
    TVOUT_MODE_1080I30, 
    TVOUT_MODE_720P50, 
    TVOUT_MODE_1080P50, 
    TVOUT_MODE_720P60, 
    TVOUT_MODE_1080P60
}
*/

static cnc_edid_vic_struct_t supported_res[]= {

    {21,   720,576,'i',       60,  4, 3, 1}, //TVOUT_MODE_576I,
    {39,  1920,1080,'i',      50, 16, 9, 0}, //TVOUT_MODE_1080I25,+
    { 0,   800,480,'p',       60,  5, 3, 0},//	TVOUT_RGB_800X480_60FPS,
    { 0,  1360,768,'p',       60, 16, 9, 0},//	TVOUT_RGB_1360X768_60FPS,
    { 0,  1366,768,'p',       60, 16, 9, 0},//	TVOUT_RGB_1366X768_60FPS,
    { 0,  1920,1080,'p',      25, 16, 9, 0}, //TVOUT_MODE_1080P25,    
    { 6,   720,480,'i',       60,  4, 3, 1}, //TVOUT_MODE_480I,
	{ 5,  1920,1080,'i',      60, 16, 9, 0}, //TVOUT_MODE_1080I30,
	{ 1,   640,480,'p',       60,  4, 3, 0}, //TVOUT_RGB_640X480_60FPS,
	{ 2,   720,480,'p',       60,  4, 3, 0}, //TVOUT_MODE_480P,
	{17,   720,576,'p',       50,  4, 3, 0}, //TVOUT_MODE_576P,
	{ 0,   800,600,'p',       60,  4, 3, 0}, //TVOUT_RGB_800X600_60FPS,
	{ 0,  1024,768,'p',       60,  4, 3, 0}, //TVOUT_RGB_1024X768_60FPS,
	{ 0,  1280,1024,'p',      60,  4, 3, 0}, //TVOUT_RGB_1280X1024_60FPS,    
    { 0,  1440,900,'p',       60,  8, 5, 0},//	TVOUT_RGB_1440X900_60FPS,
	{19,  1280,720,'p',       50, 16, 9, 0}, //TVOUT_MODE_720P50,
	{ 4,  1280,720,'p',       60, 16, 9, 0}, //TVOUT_MODE_720P60,
	{32,  1920,1080,'p',      24, 16, 9, 0}, //TVOUT_MODE_1080P24,
	{34,  1920,1080,'p',      30, 16, 9, 0}, //TVOUT_MODE_1080P30,
	{31,  1920,1080,'p',      50, 16, 9, 0}, //TVOUT_MODE_1080P50,
	{16,  1920,1080,'p',      60, 16, 9, 0} //TVOUT_MODE_1080P60,///<Define the timing mode is 1080P@60Hz( only for HDMI interface)  /* id = 16 */
};

/* Commented out resolutions were not being used before, so I have left them out also
 * but I see not real reason for doing that.
 * will investigate and decide if it makes sense to add them or not.
 */
static int tvoutmode[]= {
    TVOUT_MODE_576I,///<Define the timing mode is 576i. /* id = 0 */
    TVOUT_MODE_1080I25,///<Define the timing mode is 1080i@25hz.  /* id = 6 */
	TVOUT_RGB_800X480_60FPS,
	TVOUT_RGB_1360X768_60FPS,
	TVOUT_RGB_1366X768_60FPS,
	TVOUT_MODE_1080P25,        
    TVOUT_MODE_480I,///<Define the timing mode is 480i.     /* id = 1 */
	TVOUT_MODE_1080I30,///<Define the timing mode is 1080i@30hz.  /* id = 7 */
	TVOUT_RGB_640X480_60FPS,///<Define the timing mode is 640*480@60fps(only for VGA interface) /* id = 17 */
	TVOUT_MODE_480P,///<Define the timing mode is 480p.     /* id = 3 */	
	TVOUT_MODE_576P,///<Define the timing mode is 576p.     /* id = 2 */
	TVOUT_RGB_800X600_60FPS,///<Define the timing mode is 800*600@60fps(only for VGA interface) /* id = 18 */
	TVOUT_RGB_1024X768_60FPS,///<Define the timing mode is 1024*768@60fps(only for VGA interface) /* id = 19 */
	TVOUT_RGB_1280X1024_60FPS,///<Define the timing mode is 1280*1024@60fps(only for VGA interface) /* id = 23 */
	TVOUT_RGB_1440X900_60FPS,
	TVOUT_MODE_720P50,///<Define the timing mode is 720p@50hz.   /* id = 4 */
	TVOUT_MODE_720P60,///<Define the timing mode is 720p@60hz.   /* id = 5 */
	TVOUT_MODE_1080P24,///<Define the timing mode is 1080P@24Hz( only for HDMI interface)  /* id = 12 */
	TVOUT_MODE_1080P30,///<Define the timing mode is 1080P@30Hz( only for HDMI interface)  /* id = 14 */
	TVOUT_MODE_1080P50,///<Define the timing mode is 1080P@50Hz( only for HDMI interface)  /* id = 15 */
	TVOUT_MODE_1080P60,///<Define the timing mode is 1080P@60Hz( only for HDMI interface)  /* id = 16 */
};

static int maxindex=0;
static int maxres=0;
static int edid_timing_is_supported(cnc_edid_t *edid, int timing_index, int* tvoutmode_supported_res) {
    int NETHD_SUPPORT_TIMINGS_COUNT = sizeof(supported_res)/sizeof(cnc_edid_vic_struct_t);
    cnc_edid_timing_t *timing = edid->timings+timing_index;
    int i, v_freq = (timing->v_frequency == 59) ? 60 : timing->v_frequency;
    for(i=0; i<NETHD_SUPPORT_TIMINGS_COUNT; i++) {
//        printf("edid: %d x %d @ %d %d\n", timing->h_active, timing->v_active, v_freq, (timing->timing_flags & EDID_DTD_FLAGS_INTERLACED));
//        printf("res:  %d x %d @ %d %d\n", supported_res[i].h_active, supported_res[i].v_active, supported_res[i].v_frequency, (supported_res[i].interlaced == 'i'));
    
        if(supported_res[i].h_active == timing->h_active &&
                supported_res[i].v_active == timing->v_active &&
                supported_res[i].v_frequency == v_freq &&
                ((supported_res[i].interlaced == 'i') ==
                 ((timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) != 0))) {
            int tvout_index = tvoutmode[i];
            if(tvout_index !=0) {
                tvoutmode_supported_res[tvout_index] = 1;
            }
            if(i > maxindex) {
                maxindex=i;
                maxres=tvoutmode[i];
            }
            //        printf("EDID_SUCCESS(%d): %d x %d %c @ %dHz != %d x %d %c @ %dHz\n",i, timing->h_active, timing->v_active, (timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) ? 'i' : 'p', timing->v_frequency, supported_res[i].h_active, supported_res[i].v_active, supported_res[i].interlaced, supported_res[i].v_frequency);
            return EDID_SUCCESS;
        }
#if 0
        else {
            printf("IN ELSE(%d): %d x %d %c @ %dHz != %d x %d %c @ %dHz\n",i, timing->h_active, timing->v_active, (timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) ? 'i' : 'p', timing->v_frequency, supported_res[i].h_active, supported_res[i].v_active, supported_res[i].interlaced, supported_res[i].v_frequency);
        }
#endif
        /*
            if (supported_res[i].h_active > timing->h_active)
                break;
        */
    }
    return EDID_ERROR;
}

int cnc_edid_filter(cnc_edid_t *edid, int* tvoutmode_supported_res, int* audmode_supported) {
    int i;
    maxres = 0;
    maxindex = 0;
    memset(tvoutmode_supported_res, 0, TVOUT_MODE_MAX*sizeof(int));
    memset(audmode_supported, 0, 32*sizeof(int));
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++) {
        if(edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
            if(edid_timing_is_supported(edid, i, tvoutmode_supported_res) != EDID_SUCCESS) {
                edid->timings[i].internal_flags &= ~EDID_TIMING_IFLAGS_ENTRY_VALID;
            }
        }
    }
    for(i=0; i<EDID_AUDIO_STORAGE_SIZE; i++)
        if(edid->audio_formats[i].format != EDID_AUDIO_FORMAT_RESERVED) {
            if(edid->audio_formats[i].format == EDID_AUDIO_FORMAT_E_AC3) {
                audmode_supported[CSAUD_HDMI_DOLBY_DIGITAL_PLUS] = 1;
            }
            else if(edid->audio_formats[i].format == EDID_AUDIO_FORMAT_AC3) {
                audmode_supported[CSAUD_HDMI_DOLBY_DIGITAL] = 1;
            }
            else if(edid->audio_formats[i].format == EDID_AUDIO_FORMAT_LPCM) {
                audmode_supported[CSAUD_HDMI_PCM_DOWN_MIX] = 1;
                if(edid->audio_formats[i].max_channels>=2) {
                    audmode_supported[CSAUD_HDMI_PCM_MULTI_CHANNEL] = 1;
                }
            }
        
            // invalidate non-LPCM formats
            if(edid->audio_formats[i].format != EDID_AUDIO_FORMAT_LPCM) {
                edid->audio_formats[i].format = EDID_AUDIO_FORMAT_RESERVED;
                continue;
            }
            // max of two channels for LPCM
            //edid->audio_formats[i].max_channels = 2;
            // disallow > 48khz formats
            edid->audio_formats[i].frequencies &= EDID_EXT_BLOCK_SAD_FILTER_48khz_max;
        }
    if(edid->edid_valid & EDID_VALID_VENDOR_SPECIFIC &&
            edid->vendor_specific_length > EDID_VENDOR_SPECIFIC_FILTER_BYTE) {
        //printk("%s: vs: 0x%02x", __FUNCTION__, edid->vendor_specific[EDID_VENDOR_SPECIFIC_FILTER_BYTE]);
        edid->vendor_specific[EDID_VENDOR_SPECIFIC_FILTER_BYTE] &=
            EDID_VENDOR_SPECIFIC_FILTER_DEEP_COLOR;
        //printk(" after filter: 0x%02x\n", edid->vendor_specific[EDID_VENDOR_SPECIFIC_FILTER_BYTE]);
    }
    return(maxres);
}
