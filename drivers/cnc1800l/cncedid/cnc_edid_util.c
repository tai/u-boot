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

void cnc_edid_set_id_string(cnc_edid_t *edid, char *edid_id_string) {
    strncpy(edid->id_string, edid_id_string, EDID_ID_STRING_LENGTH);
    if(strlen(edid_id_string) >= EDID_ID_STRING_LENGTH) {
        edid->id_string[EDID_ID_STRING_LENGTH-1] = '\0';
    }
}
#if 0
int cnc_edid_read_hex(cnc_edid_t *edid) {
    int i=0, tmp;
    while(scanf("%x ", &tmp) == 1) {
        edid->edid_bin[i++] = tmp & 0xff;
        if(i >= EDID_MAX_BIN_LENGTH) {
            printf("%s: edid length limited to %d bytes\n", __FUNCTION__, EDID_MAX_BIN_LENGTH);
            return EDID_ERROR;
        }
    }
    edid->edid_bin_valid = EDID_BIN_VALID;
    edid->edid_bin_length = i;
#ifdef EDID_DEBUG
    printf("%s: read %d bytes\n", __FUNCTION__, i);
#endif
    return EDID_SUCCESS;
}
#endif
char *cnc_edid_color_bit_depth_ascii(cnc_edid_t *edid) {
    switch(edid->color_bit_depth) {
    case 0:
        return "Undefined";
    case 1:
        return "6 bits per primary color";
    case 2:
        return "8 bits per primary color";
    case 3:
        return "10 bits per primary color";
    case 4:
        return "12 bits per primary color";
    case 5:
        return "14 bits per primary color";
    case 6:
        return "16 bits per primary color";
    case 7:
        return "Reserved (should not be used)";
    default:
        return "WARNING: color_bit_depth variable corrupt";
    }
}

int cnc_edid_int_to_color_bit_depth(int color_bit_depth) {
    switch(color_bit_depth) {
    case 0:
        return 0;
    case 6:
        return 1;
    case 8:
        return 2;
    case 10:
        return 3;
    case 12:
        return 4;
    case 14:
        return 5;
    case 16:
        return 6;
    default:
        printf("%s: WARNING: color_bit_depth value out of range\n", __FUNCTION__);
    }
    return EDID_ERROR;
}

char *cnc_edid_interface_std_ascii(cnc_edid_t *edid) {
    switch(edid->interface_std) {
    case 0:
        return "Digital Interface Std is not defined";
    case 1:
        return "DVI is supported";
    case 2:
        return "HDMI-a is supported";
    case 3:
        return "HDMI-b is supported";
    case 4:
        return "HDMI-c is supported";
    case 5:
        return "Display Port is supported";
    default:
        return "Reserved (should not be used)";
    }
}

// even though I didn't save the code that used this in cnc_edid_filter.c
#ifdef EDID_FLOATING_POINT_AVAILABLE
float cnc_edid_gamma_to_f(cnc_edid_t *edid) {
    return (edid->gamma+100.0)/100.0;
}

int cnc_edid_gamme_from_f(float gamma) {
    if(gamma < 1.0 || gamma > 3.54) {
        return EDID_ERROR;
    }
    return (int)(gamma * 100) - 100;
}
#else
int cnc_edid_gamma_int(cnc_edid_t *edid) {
    return (edid->gamma + 100) / 100;
}
int cnc_edid_gamma_fract(cnc_edid_t *edid) {
    return (edid->gamma + 100) % 100;
}
#endif


char *cnc_edid_display_color_type_to_ascii(cnc_edid_t *edid) {
    int color_type;
    color_type = (edid->feature_support & EDID_VIDEO_INPUT_COLOR_TYPE_MASK);
    color_type >>= EDID_VIDEO_INPUT_COLOR_TYPE_SHIFT;
    if(edid->signal_interface == EDID_VIDEO_INPUT_DEF_SIGNAL_INT_ANALOG) {
        switch(color_type) {
        case 0:
            return "Monochrome or Grayscale display";
        case 1:
            return "RGB display";
        case 2:
            return "Non-RGB display";
        case 3:
            return "Display Color Type is Undefined";
        }
    }
    else {
        switch(color_type) {
        case 0:
            return "RGB 4:4:4";
        case 1:
            return "RGB 4:4:4 & YCbCr 4:4:4";
        case 2:
            return "RGB 4:4:4 & YCbCr 4:2:2";
        case 3:
            return "RGB 4:4:4 & YCbCr 4:4:4 & YCbCr 4:2:2";
        }
    }
    return "bug in cnc_edid_display_color_type_to_ascii()";
}

// even though I didn't save the code that used this in cnc_edid_filter.c
#ifdef EDID_FLOATING_POINT_AVAILABLE
float cnc_edid_chromaticity_to_f(int chromaticity_val) {
    return chromaticity_val/1024.0;
}

int cnc_edid_chromaticity_from_f(float chromaticity_val) {
    return (int)(chromaticity_val * 1024);
}
#else
int cnc_edid_chromaticity_fract(int chromaticity_val) {
    return (chromaticity_val * 1000) / 1024;
}
#endif

//    cnc_edid_timing_t *timing = cnc_edid_get_free_timing_struct(edid);
cnc_edid_timing_t *cnc_edid_get_free_timing_struct(cnc_edid_t *edid) {
    int i;
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if(!(edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID)) {
            //printf("%s: %d\n", __FUNCTION__, i);
            return &(edid->timings[i]);
        }
    printf("%s: failed to allocate a timing struct\n", __FUNCTION__);
    return -1;
}

//    cnc_edid_audio_t *audio_fmt = cnc_edid_get_free_audio_struct(edid);
cnc_edid_audio_t *cnc_edid_get_free_audio_struct(cnc_edid_t *edid) {
    int i;
    for(i=0; i<EDID_AUDIO_STORAGE_SIZE; i++)
        if(edid->audio_formats[i].format == EDID_AUDIO_FORMAT_RESERVED) {
            //printf("%s: %d\n", __FUNCTION__, i);
            return &(edid->audio_formats[i]);
        }
    printf("%s: failed to allocate an audio struct\n", __FUNCTION__);
    return -1;
}

char *cnc_edid_timing_iflags_location_to_ascii(cnc_edid_timing_t *timing) {
    int loc = timing->internal_flags & EDID_TIMING_IFLAGS_LOCATION_MASK;
    switch(loc) {
    case 0x01:
        return "EST";
    case 0x02:
        return "STD";
    case 0x04:
        return "EDID DTD";
    case 0x08:
        return "EXT SVD";
    case 0x10:
        return "EXT DTD";
    case 0x20:
        return "EDID 18B STD";
    default:
        printf("%s: WARNING! location field not understood or corrupt (0x%x)\n", __FUNCTION__, loc);
        return "???";
    }
}

/* CEA 861 Table 4 Video FormatsVideo ID Code and Aspect Ratios
VIC Resolution     V Rate       AR   Pixel AR
1   640x480p       59.94Hz/60Hz 4:3  1:1
2   720x480p       59.94Hz/60Hz 4:3  8:9
3   720x480p       59.94Hz/60Hz 16:9 32:27
4   1280x720p      59.94Hz/60Hz 16:9 1:1
5   1920x1080i     59.94Hz/60Hz 16:9 1:1
6   720(1440)x480i 59.94Hz/60Hz 4:3  8:9
7   720(1440)x480i 59.94Hz/60Hz 16:9 32:27
8   720(1440)x240p 59.94Hz/60Hz 4:3  4:9
9   720(1440)x240p 59.94Hz/60Hz 16:9 16:27
10  2880x480i      59.94Hz/60Hz 4:3  2:9  20:92
11  2880x480i      59.94Hz/60Hz 16:9 8:27 -80:272
12  2880x240p      59.94Hz/60Hz 4:3  1:9  10:92
13  2880x240p      59.94Hz/60Hz 16:9 4:27  40:272
14  1440x480p      59.94Hz/60Hz 4:3  4:9 or 8:93
15  1440x480p      59.94Hz/60Hz 16:9 16:27 or 32:273
16  1920x1080p     59.94Hz/60Hz 16:9 1:1
17  720x576p       50Hz         4:3  16:15
18  720x576p       50Hz         16:9 64:45
19  1280x720p      50Hz         16:9 1:1
20  1920x1080i     50Hz         16:9 1:1
21  720(1440)x576i 50Hz         4:3  16:15
22  720(1440)x576i 50Hz         16:9 64:45
23  720(1440)x288p 50Hz         4:3  8:15
24  720(1440)x288p 50Hz         16:9 32:45
25  2880x576i      50Hz         4:3  2:15  20:152
26  2880x576i      50Hz         16:9 16:45-160:452
27  2880x288p      50Hz         4:3  1:15  10:152
28  2880x288p      50Hz         16:9 8:45  80:452
29  1440x576p      50Hz         4:3  8:15 or 16:153
30  1440x576p      50Hz         16:9 32:45 or 64:453
31  1920x1080p     50Hz         16:9 1:1
32  1920x1080p     23.97Hz/24Hz 16:9 1:1
33  1920x1080p     25Hz         16:9 1:1
34  1920x1080p     29.97Hz/30Hz 16:9 1:1
35  2880x480p      59.94Hz/60Hz 4:3  2:9, 4:9, or 8:94
36  2880x480p      59.94Hz/60Hz 16:9 8:27, 16:27, or 32:274
37  2880x576p      50Hz         4:3  4:15, 8:15, or 16:154
38  2880x576p      50Hz         16:9 16:45, 32:45, or 64:454
39  1920x1080i     50Hz         16:9 1:1
40  1920x1080i     100Hz        16:9 1:1
41  1280x720p      100Hz        16:9 1:1
42  720x576p       100Hz        4:3  16:15
43  720x576p       100Hz        16:9 64:45
44  720(1440)x576i 100Hz        4:3  16:15
45  720(1440)x576i 100Hz        16:9 64:45
46  1920x1080i     119.88/120Hz 16:9 1:1
47  1280x720p      119.88/120Hz 16:9 1:1
48  720x480p       119.88/120Hz 4:3  8:9
49  720x480p       119.88/120Hz 16:9 32:27
50  720(1440)x480i 119.88/120Hz 4:3  8:9
51  720(1440)x480i 119.88/120Hz 16:9 32:27
52  720x576p       200Hz        4:3  16:15
53  720x576p       200Hz        16:9 64:45
54  720(1440)x576i 200Hz        4:3  16:15
55  720(1440)x576i 200Hz        16:9 64:45
56  720x480p       239.76/240Hz 4:3  8:9
57  720x480p       239.76/240Hz 16:9 32:27
58  720(1440)x480i 239.76/240Hz 4:3  8:9
59  720(1440)x480i 239.76/240Hz 16:9 32:27
60  1280x720p      23.97Hz/24Hz 16:9 1:1
61  1280x720p      25Hz         16:9 1:1
62  1280x720p      29.97Hz/30Hz 16:9 1:1
63  1920x1080p     119.88/120Hz 16:9 1:1
64  1920x1080p     100Hz        16:9 1:1
*/


static cnc_edid_vic_struct_t edid_vic[EDID_MAX_VIC+1]= {
    { 0,   0,0,'p',            0,  0, 0, 0},
    { 1,   640,480,'p',       60,  4, 3, 0},
    { 2,   720,480,'p',       60,  4, 3, 0},
    { 3,   720,480,'p',       60, 16, 9, 0},
    { 4,   1280,720,'p',      60, 16, 9, 0},
    { 5,   1920,1080,'i',     60, 16, 9, 0},
    { 6,   720,480,'i',       60,  4, 3, 1},
    { 7,   720,480,'i',       60, 16, 9, 1},
    { 8,   720,240,'p',       60,  4, 3, 1},
    { 9,   720,240,'p',       60, 16, 9, 1},
    {10,  2880,480,'i',       60,  4, 3, 0},
    {11,  2880,480,'i',       60, 16, 9, 0},
    {12,  2880,240,'p',       60,  4, 3, 0},
    {13,  2880,240,'p',       60, 16, 9, 0},
    {14,  1440,480,'p',       60,  4, 3, 0},
    {15,  1440,480,'p',       60, 16, 9, 0},
    {16,  1920,1080,'p',      60, 16, 9, 0},
    {17,  720,576,'p',        50,  4, 3, 0},
    {18,  720,576,'p',        50, 16, 9, 0},
    {19,  1280,720,'p',       50, 16, 9, 0},
    {20,  1920,1080,'i',      50, 16, 9, 0},
    {21,  720,576,'i',        50,  4, 3, 1},
    {22,  720,576,'i',        50, 16, 9, 1},
    {23,  720,288,'p',        50,  4, 3, 1},
    {24,  720,288,'p',        50, 16, 9, 1},
    {25,  2880,576,'i',       50,  4, 3, 0},
    {26,  2880,576,'i',       50, 16, 9, 0},
    {27,  2880,288,'p',       50,  4, 3, 0},
    {28,  2880,288,'p',       50, 16, 9, 0},
    {29,  1440,576,'p',       50,  4, 3, 0},
    {30,  1440,576,'p',       50, 16, 9, 0},
    {31,  1920,1080,'p',      50, 16, 9, 0},
    {32,  1920,1080,'p',      24, 16, 9, 0},
    {33,  1920,1080,'p',      25, 16, 9, 0},
    {34,  1920,1080,'p',      30, 16, 9, 0},
    {35,  2880,480,'p',       60,  4, 3, 0},
    {36,  2880,480,'p',       60, 16, 9, 0},
    {37,  2880,576,'p',       50,  4, 3, 0},
    {38,  2880,576,'p',       50, 16, 9, 0},
    {39,  1920,1080,'i',      50, 16, 9, 0},
    {40,  1920,1080,'i',     100, 16, 9, 0},
    {41,  1280,720,'p',      100, 16, 9, 0},
    {42,  720,576,'p',       100,  4, 3, 0},
    {43,  720,576,'p',       100, 16, 9, 0},
    {44,  720,576,'i',       100,  4, 3, 1},
    {45,  720,576,'i',       100, 16, 9, 1},
    {46,  1920,1080,'i',     120, 16, 9, 0},
    {47,  1280,720,'p',      120, 16, 9, 0},
    {48,  720,480,'p',       120,  4, 3, 0},
    {49,  720,480,'p',       120, 16, 9, 0},
    {50,  720,480,'i',       120,  4, 3, 1},
    {51,  720,480,'i',       120, 16, 9, 1},
    {52,  720,576,'p',       200,  4, 3, 0},
    {53,  720,576,'p',       200, 16, 9, 0},
    {54,  720,576,'i',       200,  4, 3, 1},
    {55,  720,576,'i',       200, 16, 9, 1},
    {56,  720,480,'p',       240,  4, 3, 0},
    {57,  720,480,'p',       240, 16, 9, 0},
    {58,  720,480,'i',       240,  4, 3, 1},
    {59,  720,480,'i',       240, 16, 9, 1},
    {60,  1280,720,'p',       24, 16, 9, 0},
    {61,  1280,720,'p',       25, 16, 9, 0},
    {62,  1280,720,'p',       30, 16, 9, 0},
    {63,  1920,1080,'p',     120, 16, 9, 0},
    {64,  1920,1080,'p',     100, 16, 9, 0}
};

int cnc_edid_vic_to_edid_timing(cnc_edid_timing_t *timing, int vic) {
    if(vic == 0) { // have seen this; LGFlatron EDID; ignore silently
        return EDID_SUCCESS;
    }
    if(vic < EDID_MIN_VIC || vic > EDID_MAX_VIC) {
        printf("%s: WARNING, VIC out of range (%d)\n", __FUNCTION__, vic);
        return EDID_ERROR;
    }
    if(edid_vic[vic].vic != vic) {
        printf("%s: WARNING, problem with VIC table\n", __FUNCTION__);
        return EDID_ERROR;
    }
    timing->vic            = vic;
    timing->h_active       = edid_vic[vic].h_active;
    timing->v_active       = edid_vic[vic].v_active;
    if(edid_vic[vic].interlaced == 'i') {
        timing->timing_flags |= EDID_DTD_FLAGS_INTERLACED;
    }
    timing->v_frequency    = edid_vic[vic].v_frequency;
    timing->ar_numerator   = edid_vic[vic].ar_numerator;
    timing->ar_denominator = edid_vic[vic].ar_denominator;
    if(edid_vic[vic].pixel_doubled) {
        timing->internal_flags |= EDID_TIMING_IFLAGS_PIXEL_DOUBLED;
    }
    timing->internal_flags |=
        (EDID_TIMING_IFLAGS_LOCATION_SVD |
         EDID_TIMING_IFLAGS_AR_VALID |
         EDID_TIMING_IFLAGS_V_FREQ_VALID |
         EDID_TIMING_IFLAGS_VIC_VALID |
         EDID_TIMING_IFLAGS_ENTRY_VALID);
    return EDID_SUCCESS;
}
