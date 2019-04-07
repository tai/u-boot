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

int cnc_edid_print_hex(cnc_edid_t *edid) {
    int i=0, edid_retval=EDID_SUCCESS;
    EDID_VALIDATE_BINARY(edid, 0);
    printf("\n");
    for(i=0; i<edid->edid_bin_length; i++) {
        if((i & 0xf) == 0) {
            printf("0x%02x:", i);
        }
        printf(" %02x", edid->edid_bin[i]);
        if((i & 0xf) == 0xf) {
            printf("\n");
        }
    }
    printf("\n");
    return edid_retval;
}

int cnc_edid_print_vendor_product_id(cnc_edid_t *edid) {
    if(edid->edid_valid & EDID_VALID_VENDOR_PRODUCT_ID) {
        printf("\t   Manufacturer: %s\n", edid->id_mfg_name);
        printf("\t   Product Code: 0x%04x\n", edid->id_product_code);
        printf("\t  Serial Number: 0x%08x\n", edid->id_serial_number);
        printf("\t    Week of Mfg: %d\n", edid->week_of_mfg);
        printf("\t    Year of Mfg: %d\n", edid->year_of_mfg+1990);
    }
    // for debugging id mfg name parsing
    //printf("%02x %02x %02x %02x\n", edid->id_mfg_name[0], edid->id_mfg_name[1], edid->id_mfg_name[2], edid->id_mfg_name[3]);
    return EDID_SUCCESS;
}

int cnc_edid_print_version(cnc_edid_t *edid) {
    if(edid->edid_valid & EDID_VALID_VERSION) {
        printf("\t        Version: %d\n", edid->version);
        printf("\t       Revision: %d\n", edid->revision);
    }
    return EDID_SUCCESS;
}

int cnc_edid_print_display_params(cnc_edid_t *edid) {
    if(edid->edid_valid & EDID_VALID_DISPLAY_PARAMS) {
        if(edid->signal_interface == EDID_VIDEO_INPUT_DEF_SIGNAL_INT_DIGITAL) {
            printf("\t      Interface: Digital\n");
            printf("\tColor Bit Depth: %s\n", cnc_edid_color_bit_depth_ascii(edid));
            printf("\t  Interface Std: %s\n", cnc_edid_interface_std_ascii(edid));
        }
        else {
            printf("\t      Interface: Analog\n");
            printf("\t    Field Value: 0x%02x (should parse)\n", edid->video_input_def);
        }
        if(edid->horizontal != 0 && edid->vertical != 0) {
            printf("\t      Horzontal: %3dcm %d.%02din\n", edid->horizontal, (edid->horizontal*100)/254, edid->horizontal%254);
            printf("\t       Vertical: %3dcm %d.%02din\n", edid->vertical, (edid->vertical*100)/254, edid->vertical%254);
        }
        else {
            printf("\t     H/V Fields: 0x%02x 0x%02x\n", edid->horizontal, edid->vertical);
        }
        if(edid->gamma != 0xff) {
            printf("\t          Gamma: %d.%02d\n", cnc_edid_gamma_int(edid), cnc_edid_gamma_fract(edid));
        }
        else {
            printf("\t          Gamma: Undefined\n");
        }
        printf("\t        Standby: %supported\n",
               edid->feature_support & EDID_VIDEO_INPUT_STANDBY_MASK ? "S" : "Uns");
        printf("\t        Suspend: %supported\n",
               edid->feature_support & EDID_VIDEO_INPUT_SUSPEND_MASK ? "S" : "Uns");
        printf("\t     Active Off: %supported\n",
               edid->feature_support & EDID_VIDEO_INPUT_ACTIVE_OFF_MASK ? "S" : "Uns");
        printf("\t     Color Type: %s\n", cnc_edid_display_color_type_to_ascii(edid));
        printf("\t       sRGB Std: is %sthe default color space\n", (edid->feature_support & EDID_VIDEO_INPUT_SRGB_MASK) ? "" : "not ");
        printf("\t Preferred Mode: %s the native pixel format and preferred refresh rate\n", (edid->feature_support & EDID_VIDEO_INPUT_PREFERRED_MASK) ? "includes" : "does not include");
        printf("\t Continous Freq: Display is %scontinuous frequency %s\n", (edid->feature_support & EDID_VIDEO_INPUT_CONTINUOUS_MASK) ? "" : "non-", (edid->feature_support & EDID_VIDEO_INPUT_CONTINUOUS_MASK) ? "" : "(multi-mode)");
    }
    return EDID_SUCCESS;
}

int cnc_edid_print_chromaticity(cnc_edid_t *edid) {
    if(edid->edid_valid & EDID_VALID_CHROMATICITY) {
        printf("\t Chromaticity R: x 0.%03d, y 0.%03d\n",
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Rx_INDEX]),
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Ry_INDEX]));
        printf("\t              G: x 0.%03d, y 0.%03d\n",
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Gx_INDEX]),
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Gy_INDEX]));
        printf("\t              B: x 0.%03d, y 0.%03d\n",
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Bx_INDEX]),
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_By_INDEX]));
        printf("\t              W: x 0.%03d, y 0.%03d\n",
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Wx_INDEX]),
               cnc_edid_chromaticity_fract(edid->chromaticity[EDID_CHROMATICITY_Wy_INDEX]));
    }
    return EDID_SUCCESS;
}

void cnc_edid_print_timing(cnc_edid_timing_t *timing, char *prefix) {
    printf("%s %4dx%d%c @ %dHz\n", prefix,
           timing->h_active,
           timing->v_active,
           (timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) ? 'i' : 'p',
           timing->v_frequency);
}

int cnc_edid_print_established_timings(cnc_edid_t *edid) {
    char *prefix_on ="\t    Est Timings: ";
    char *prefix_off="\t               : ";
    char *prefix=prefix_on;
    int i;
    if(!(edid->edid_valid & EDID_VALID_EST_TIMINGS)) {
        return EDID_ERROR;
    }
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if((edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) &&
                (edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_LOCATION_EST)) {
            cnc_edid_print_timing(&(edid->timings[i]), prefix);
            prefix = prefix_off;
        }
    return EDID_SUCCESS;
}

int cnc_edid_print_standard_timings(cnc_edid_t *edid) {
    char *prefix_on ="\t    Std Timings: ";
    char *prefix_off="\t               : ";
    char *prefix=prefix_on;
    int i;
    if(!(edid->edid_valid & EDID_VALID_STD_TIMINGS)) {
        return EDID_ERROR;
    }
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if((edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) &&
                (edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_LOCATION_STD)) {
            cnc_edid_print_timing(&(edid->timings[i]), prefix);
            prefix = prefix_off;
        }
    return EDID_SUCCESS;
}

int cnc_edid_print_display_desc_string(char *stored_str) {
    char print_str[EDID_DISPLAY_DESC_STRING_STORAGE_SIZE];
    char *newline;
    strncpy(print_str, stored_str, EDID_DISPLAY_DESC_STRING_LENGTH);
    // supposed to be terminated by a newline and padded with spaces
    // check anyway and remove the spaces
#if 0
	newline = index(print_str, '\n');
    if(newline == -1) {
        print_str[EDID_DISPLAY_DESC_STRING_LENGTH] = '\0';
    }
    else {
        *newline = '\0';
    }
#endif
    printf("%s\n", print_str);
    return EDID_SUCCESS;
}

int cnc_edid_print_display_serial_number(cnc_edid_t *edid) {
    if(!(edid->edid_valid & EDID_VALID_DISPLAY_SERIAL_NUMBER)) {
        return EDID_ERROR;
    }
    printf("\t  Serial Number: ");
    return cnc_edid_print_display_desc_string(edid->display_serial_number);
}

int cnc_edid_print_display_data_string(cnc_edid_t *edid) {
    if(!(edid->edid_valid & EDID_VALID_DISPLAY_DATA_STR)) {
        return EDID_ERROR;
    }
    printf("\t    Data String: ");
    return cnc_edid_print_display_desc_string(edid->display_data_string);
}

int cnc_edid_print_display_product_name(cnc_edid_t *edid) {
    if(!(edid->edid_valid & EDID_VALID_DISPLAY_PRODUCT_NAME)) {
        return EDID_ERROR;
    }
    printf("\t   Product Name: ");
    return cnc_edid_print_display_desc_string(edid->display_product_name);
}

int cnc_edid_print_display_range_limits(cnc_edid_t *edid) {
    if(!(edid->edid_valid & EDID_VALID_DISPLAY_RANGE_LIMITS)) {
        return EDID_ERROR;
    }
    printf("\t Disp Range Lim: V min %2d Hz,  max %3d Hz\n",
           edid->rate_v_min, edid->rate_v_max);
    printf("\t               : H min %2d kHz, max %3d kHz\n",
           edid->rate_h_min, edid->rate_h_max);
    printf("\t               : pixel clock   max %3d MHz\n", edid->rate_pixel_clock_max);
    return EDID_SUCCESS;
}

int cnc_edid_print_audio_formats(cnc_edid_t *edid) {
    char *freq_strs[EDID_AUDIO_FREQ_STRS_STORAGE_SIZE] = EDID_AUDIO_FREQUENCY_STRS_INITIALIZER;
    char *format_strs[EDID_AUDIO_FORMAT_STRS_STORAGE_SIZE] = EDID_AUDIO_FORMAT_STRS_INITIALIZER;
    cnc_edid_audio_t *audio_fmt;
    int i, f, need_comma, need_hdr=1;
    if(!(edid->edid_valid & EDID_VALID_AUDIO_FORMATS)) {
        return EDID_ERROR;
    }
    for(i=0; i<EDID_AUDIO_STORAGE_SIZE; i++) {
        audio_fmt = &(edid->audio_formats[i]);
        if(audio_fmt->format == EDID_AUDIO_FORMAT_RESERVED) {
            continue;
        }
        printf("\t  %s: %d channels of %s at\n", need_hdr?"Audio Formats":"             ", audio_fmt->max_channels, format_strs[audio_fmt->format]);
        need_hdr = 0;
        printf("\t               :     ");
        if(audio_fmt->frequencies == 0) {
            printf("Refer to data for frequency range\n");
        }
        else {
            need_comma = 0;
            for(f=0; f<EDID_AUDIO_FREQ_STRS_STORAGE_SIZE; f++) {
                if(audio_fmt->frequencies & (1<<f)) {
                    printf("%s%s", need_comma?", ":"", freq_strs[f]);
                    need_comma = 1;
                }
            }
            printf(" kHz\n");
        }
        if(audio_fmt->format == EDID_AUDIO_FORMAT_LPCM) {
            printf("\t               :     ");
            if(audio_fmt->extra_data & EDID_AUDIO_FORMAT_LPCM_16_BIT_MASK) {
                printf("16");
                need_comma = 1;
            }
            else {
                need_comma = 0;
            }
            if(audio_fmt->extra_data & EDID_AUDIO_FORMAT_LPCM_20_BIT_MASK) {
                printf("%s20", need_comma?", ":"");
                need_comma = 1;
            }
            if(audio_fmt->extra_data & EDID_AUDIO_FORMAT_LPCM_24_BIT_MASK) {
                printf("%s24", need_comma?", ":"");
            }
            printf(" bits/sample\n");
        }
        //printf("\t   Audio Format: %s\n", format_strs[audio_fmt->format]);
    }
    return EDID_SUCCESS;
}

int cnc_edid_print_video_capability(cnc_edid_t *edid) {
    char *vidcap_strs[EDID_VIDCAP_STRINGS_STORAGE_SIZE] = EDID_VIDCAP_STRS_INITIALIZER;
    if(!(edid->edid_valid & EDID_VALID_VIDEO_CAPABILITY)) {
        return EDID_ERROR;
    }
    printf("\tVideoCapability: CE Formats: %s\n", vidcap_strs[edid->video_capability & EDID_VIDCAP_S_CE_MASK]);
    printf("\t               : IT Formats: %s\n", vidcap_strs[(edid->video_capability & EDID_VIDCAP_S_IT_MASK)>>EDID_VIDCAP_S_IT_SHIFT]);
    if(edid->video_capability & EDID_VIDCAP_S_PT_MASK) {
        printf("\t               : Preferred Format: %s\n", vidcap_strs[(edid->video_capability & EDID_VIDCAP_S_PT_MASK)>>EDID_VIDCAP_S_PT_SHIFT]);
    }
    if(edid->video_capability & EDID_VIDCAP_QS_MASK) {
        printf("\t               : RGB Quant Range is Selectable via AVI Q\n");
    }
    if(edid->video_capability & EDID_VIDCAP_QY_MASK) {
        printf("\t               : YCC Quant Range is Selectable via AVI YQ\n");
    }
    return EDID_SUCCESS;
}

int cnc_edid_print_vendor_specific(cnc_edid_t *edid) {
    int i;
    if(!(edid->edid_valid & EDID_VALID_VENDOR_SPECIFIC)) {
        return EDID_ERROR;
    }
    printf("\tVendor Specific: ");
    for(i=0; i<edid->vendor_specific_length; i++) {
        printf("%02x ", edid->vendor_specific[i]);
    }
    printf("\n");
    return EDID_SUCCESS;
}

int cnc_edid_print_colorimetry(cnc_edid_t *edid) {
    int i, need_hdr=1;
    char *colorimetry_strs[EDID_COLORIMETRY_STRS_STORAGE_SIZE] = EDID_COLORIMETRY_STRS_INITIALIZER;
    if(!(edid->edid_valid & EDID_VALID_COLORIMETRY)) {
        return EDID_ERROR;
    }
    for(i=0; i<EDID_COLORIMETRY_STRS_STORAGE_SIZE-1; i++)
        if(edid->colorimetry & (1<<i)) {
            printf("\t    %s: %s\n", need_hdr?"Colorimetry":"           ", colorimetry_strs[i]);
            need_hdr = 0;
        }
    if(edid->colorimetry & EDID_COLORIMETRY_BYTE2_MASK) {
        printf("\t    %s: %s\n", need_hdr?"Colorimetry":"           ", colorimetry_strs[i]);
    }
    return EDID_SUCCESS;
}

int cnc_edid_print_speaker_alloc(cnc_edid_t *edid) {
    char *speaker_strs[EDID_SPEAKER_STRS_STORAGE_SIZE] = EDID_SPEAKER_STRS_INITIALIZER;
    int i, need_hdr=1;
    if(!(edid->edid_valid & EDID_VALID_SPEAKER_ALLOC)) {
        return EDID_ERROR;
    }
    for(i=0; i<EDID_SPEAKER_STRS_STORAGE_SIZE; i++) {
        if(edid->speaker_alloc & (1<<i)) {
            printf("\t       %s: %s\n", need_hdr?"Speakers":"        ", speaker_strs[i]);
            need_hdr=0;
        }
    }
    return EDID_SUCCESS;
}

/*
    edid->eedid_audio =
    block[EDID_EXT_BLOCK_FLAGS_OFFSET] & EDID_EXT_BLOCK_FLAGS_AUDIO_MASK;
    edid->eedid_YCbCr444 =
    block[EDID_EXT_BLOCK_FLAGS_OFFSET] & EDID_EXT_BLOCK_FLAGS_YCbCr_444_MASK;
    edid->eedid_YCbCr422 =
    block[EDID_EXT_BLOCK_FLAGS_OFFSET] & EDID_EXT_BLOCK_FLAGS_YCbCr_422_MASK;
    edid->eedid_num_dtds =
    block[EDID_EXT_BLOCK_FLAGS_OFFSET] & EDID_EXT_BLOCK_FLAGS_DTD_COUNT_MASK;
*/

// debug function
void cnc_edid_print_timings(cnc_edid_t *edid) {
    int i;
    printf("\n");
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if(edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID)
#if 1
            printf("%2d: %4d x %4d %c %dhz%c(%s) %d %d\n", i,
                   edid->timings[i].h_active,
                   edid->timings[i].v_active,
                   (edid->timings[i].timing_flags & EDID_DTD_FLAGS_INTERLACED)? 'i' : 'p',
                   (edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_V_FREQ_VALID) ?
                   edid->timings[i].v_frequency : edid->timings[i].pixel_clock,
                   (edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_PREFERRED)?'*':' ',
                   cnc_edid_timing_iflags_location_to_ascii(&(edid->timings[i])),
                   edid->timings[i].ar_numerator,
                   edid->timings[i].ar_denominator
                  );
#else
            printf("T %4d x %4d %c %dhz\n",
                   edid->timings[i].h_active,
                   edid->timings[i].v_active,
                   (edid->timings[i].timing_flags & EDID_DTD_FLAGS_INTERLACED)? 'i' : 'p',
                   (edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_V_FREQ_VALID) ?
                   edid->timings[i].v_frequency : edid->timings[i].pixel_clock);
#endif
    printf("\n");
}

int cnc_edid_print(cnc_edid_t *edid) {
    printf("\n");
    cnc_edid_print_vendor_product_id(edid);
    cnc_edid_print_version(edid);
    cnc_edid_print_display_params(edid);
    cnc_edid_print_chromaticity(edid);
    cnc_edid_print_established_timings(edid);
    cnc_edid_print_standard_timings(edid);
    cnc_edid_print_display_serial_number(edid);
    cnc_edid_print_display_data_string(edid);
    cnc_edid_print_display_product_name(edid);
    cnc_edid_print_display_range_limits(edid);
    cnc_edid_print_audio_formats(edid);
    cnc_edid_print_speaker_alloc(edid);
    cnc_edid_print_video_capability(edid);
    cnc_edid_print_vendor_specific(edid);
    cnc_edid_print_colorimetry(edid);
    cnc_edid_print_timings(edid);
    return EDID_SUCCESS;
}
