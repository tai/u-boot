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

// return checksum, which will be 0 if successful
int cnc_edid_check_sum(cnc_edid_t *edid, int which_block) {
    int i, sum=0;
    int offset=which_block*EDID_BLOCK_LENGTH;
    for(i=0; i<EDID_BLOCK_LENGTH; i++) {
        sum += edid->edid_bin[offset++];
    }
    sum &= 0xff;
    if(sum != 0) {
        printf("%s: failed for block %d, sum=0x%02x\n", __FUNCTION__, which_block, sum);
        EDID_MARK_BAD_CHECKSUM(edid, which_block);
    }
#ifdef EDID_DEBUG
    else {
        printf("%s: checksum ok for block %d\n", __FUNCTION__, which_block);
    }
#endif
    return sum;
}

int cnc_edid_parse_header(cnc_edid_t *edid) {
    unsigned char header[EDID_HEADER_LENGTH] = EDID_HEADER_INITIALIZER;
    int i;
    for(i=0; i<EDID_HEADER_LENGTH; i++)
        if(edid->edid_bin[i] != header[i]) {
            printf("%s: header byte mismatch at byte %d, wanted 0x%02x, got 0x%02x\n",
                   __FUNCTION__, i, header[i], edid->edid_bin[i]);
            return EDID_ERROR;
        }
    edid->edid_valid |= EDID_VALID_HEADER;
    EDID_DEBUG_DUMP(edid, EDID_HEADER_OFFSET, EDID_HEADER_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_parse_vendor_product_id(cnc_edid_t *edid) {
    // MFG Name, 5 bit ascii from base of 'A', so 0='A'
    edid->id_mfg_name[0] = '@'+((edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+0] & 0x7c) >> 2);
    edid->id_mfg_name[1] = '@'+
                           (((edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+0] & 0x03) << 3) |
                            ((edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+1] & 0xe0) >> 5));
    edid->id_mfg_name[2] = '@'+(edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+1] & 0x1f);
    edid->id_mfg_name[3] = '\0';
    EDID_PARSE_SHORT_TO_INT(edid->id_product_code, edid->edid_bin+EDID_ID_PRODUCT_CODE_OFFSET);
    EDID_PARSE_LONG_TO_INT(edid->id_serial_number, edid->edid_bin+EDID_ID_SERIAL_NUMBER_OFFSET);
    edid->week_of_mfg = edid->edid_bin[EDID_WEEK_OF_MFG_OFFSET];
    edid->year_of_mfg = edid->edid_bin[EDID_YEAR_OF_MFG_OFFSET];
    edid->edid_valid |= EDID_VALID_VENDOR_PRODUCT_ID;
    EDID_DEBUG_DUMP(edid, EDID_VENDOR_PRODUCT_ID_OFFSET, EDID_VENDOR_PRODUCT_ID_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_parse_version(cnc_edid_t *edid) {
    edid->version = edid->edid_bin[EDID_VERSION_OFFSET];
    edid->revision = edid->edid_bin[EDID_REVISION_OFFSET];
    edid->edid_valid |= EDID_VALID_VERSION;
    EDID_DEBUG_DUMP(edid, EDID_VERSION_OFFSET, EDID_VERSION_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_parse_display_params(cnc_edid_t *edid) {
    edid->signal_interface = edid->edid_bin[EDID_VIDEO_INPUT_DEF_OFFSET] &
                             EDID_VIDEO_INPUT_DEF_SIGNAL_INT_MASK;
    if(edid->signal_interface == EDID_VIDEO_INPUT_DEF_SIGNAL_INT_DIGITAL) {
        edid->color_bit_depth =
            (edid->edid_bin[EDID_VIDEO_INPUT_DEF_OFFSET] &
             EDID_VIDEO_INPUT_DEF_COLOR_DEPTH_MASK) >>
            EDID_VIDEO_INPUT_DEF_COLOR_DEPTH_SHIFT;
        edid->interface_std =
            (edid->edid_bin[EDID_VIDEO_INPUT_DEF_OFFSET] &
             EDID_VIDEO_INPUT_DEF_INTERFACE_STD_MASK) >>
            EDID_VIDEO_INPUT_DEF_INTERFACE_STD_SHIFT;
    }
    else {   // EDID_VIDEO_INPUT_DEF_SIGNAL_INT_ANALOG
        edid->video_input_def = edid->edid_bin[EDID_DISPLAY_PARAMS_OFFSET];
    }
    edid->horizontal = edid->edid_bin[EDID_VIDEO_INPUT_HORIZONTAL_OFFSET];
    edid->vertical = edid->edid_bin[EDID_VIDEO_INPUT_VERTICAL_OFFSET];
    edid->gamma = edid->edid_bin[EDID_VIDEO_INPUT_GAMMA_OFFSET];
    edid->feature_support = edid->edid_bin[EDID_VIDEO_INPUT_FEATURE_SUPPORT_OFFSET];
    edid->edid_valid |= EDID_VALID_DISPLAY_PARAMS;
    EDID_DEBUG_DUMP(edid, EDID_DISPLAY_PARAMS_OFFSET, EDID_DISPLAY_PARAMS_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_parse_chromaticity(cnc_edid_t *edid) {
    edid->chromaticity[EDID_CHROMATICITY_Rx_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_RxLo_OFFSET] & EDID_CHROMATICITY_RxLo_MASK) >>
         EDID_CHROMATICITY_RxLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_RxHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Ry_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_RyLo_OFFSET] & EDID_CHROMATICITY_RyLo_MASK) >>
         EDID_CHROMATICITY_RyLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_RyHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Gx_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_GxLo_OFFSET] & EDID_CHROMATICITY_GxLo_MASK) >>
         EDID_CHROMATICITY_GxLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_GxHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Gy_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_GyLo_OFFSET] & EDID_CHROMATICITY_GyLo_MASK) >>
         EDID_CHROMATICITY_GyLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_GyHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Bx_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_BxLo_OFFSET] & EDID_CHROMATICITY_BxLo_MASK) >>
         EDID_CHROMATICITY_BxLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_BxHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_By_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_ByLo_OFFSET] & EDID_CHROMATICITY_ByLo_MASK) >>
         EDID_CHROMATICITY_ByLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_ByHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Wx_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_WxLo_OFFSET] & EDID_CHROMATICITY_WxLo_MASK) >>
         EDID_CHROMATICITY_WxLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_WxHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->chromaticity[EDID_CHROMATICITY_Wy_INDEX] =
        ((edid->edid_bin[EDID_CHROMATICITY_WyLo_OFFSET] & EDID_CHROMATICITY_WyLo_MASK) >>
         EDID_CHROMATICITY_WyLo_SHIFT) |
        (edid->edid_bin[EDID_CHROMATICITY_WyHi_OFFSET] << EDID_CHROMATICITY_HiByte_SHIFT);
    edid->edid_valid |= EDID_VALID_CHROMATICITY;
    EDID_DEBUG_DUMP(edid, EDID_CHROMATICITY_OFFSET, EDID_CHROMATICITY_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_save_established_timing(cnc_edid_t *edid, int h, int v, int hz, int interlaced) {
    cnc_edid_timing_t *timing = cnc_edid_get_free_timing_struct(edid);
    if(timing == -1) {
        return EDID_ERROR;
    }
    timing->h_active = h;
    timing->v_active = v;
    timing->v_frequency = hz;
    timing->timing_flags = interlaced ? EDID_DTD_FLAGS_INTERLACED : 0;
    timing->internal_flags =
        EDID_TIMING_IFLAGS_LOCATION_EST |
        EDID_TIMING_IFLAGS_V_FREQ_VALID |
        EDID_TIMING_IFLAGS_ENTRY_VALID;
    return EDID_SUCCESS;
}

int cnc_edid_parse_established_timings(cnc_edid_t *edid) {
    unsigned char et1=edid->edid_bin[EDID_ESTABLISHED_TIMINGS_I_OFFSET];
    unsigned char et2=edid->edid_bin[EDID_ESTABLISHED_TIMINGS_II_OFFSET];
    unsigned char mt =edid->edid_bin[EDID_ESTABLISHED_TIMINGS_MFG_OFFSET];
    if(et1 & 0x80) {
        cnc_edid_save_established_timing(edid, 720, 400, 70, 0);
    }
    if(et1 & 0x40) {
        cnc_edid_save_established_timing(edid, 720, 400, 88, 0);
    }
    if(et1 & 0x20) {
        cnc_edid_save_established_timing(edid, 640, 480, 60, 0);
    }
    if(et1 & 0x10) {
        cnc_edid_save_established_timing(edid, 640, 480, 67, 0);
    }
    if(et1 & 8) {
        cnc_edid_save_established_timing(edid, 640, 480, 72, 0);
    }
    if(et1 & 4) {
        cnc_edid_save_established_timing(edid, 640, 480, 75, 0);
    }
    if(et1 & 2) {
        cnc_edid_save_established_timing(edid, 800, 600, 56, 0);
    }
    if(et1 & 1) {
        cnc_edid_save_established_timing(edid, 800, 600, 60, 0);
    }
    if(et2 & 0x80) {
        cnc_edid_save_established_timing(edid, 800, 600, 72, 0);
    }
    if(et2 & 0x40) {
        cnc_edid_save_established_timing(edid, 800, 600, 75, 0);
    }
    if(et2 & 0x20) {
        cnc_edid_save_established_timing(edid, 832, 624, 75, 0);
    }
    if(et2 & 0x10) {
        cnc_edid_save_established_timing(edid, 1024, 768, 87, 1);
    }
    if(et2 & 8) {
        cnc_edid_save_established_timing(edid, 1024, 768, 60, 0);
    }
    if(et2 & 4) {
        cnc_edid_save_established_timing(edid, 1024, 768, 70, 0);
    }
    if(et2 & 2) {
        cnc_edid_save_established_timing(edid, 1024, 768, 75, 0);
    }
    if(et2 & 1) {
        cnc_edid_save_established_timing(edid, 1280, 1024, 75, 0);
    }
    if(mt & 0x80) {
        cnc_edid_save_established_timing(edid, 1152, 870, 75, 0);
    }
#ifdef EDID_DEPRECATED
    memcpy(edid->edid_established_timings,
           edid->edid_bin+EDID_ESTABLISHED_TIMINGS_OFFSET,
           EDID_ESTABLISHED_TIMINGS_LENGTH);
#endif
    edid->edid_valid |= EDID_VALID_EST_TIMINGS;
    EDID_DEBUG_DUMP(edid, EDID_ESTABLISHED_TIMINGS_OFFSET, EDID_ESTABLISHED_TIMINGS_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_parse_and_store_standard_timing(cnc_edid_t *edid, int offset, cnc_edid_timing_t *timing) {
    unsigned char b0 = edid->edid_bin[offset];
    unsigned char b1 = edid->edid_bin[offset+1];
    if(b0 == 1 && b1 == 1) { // this std timing block is unused
        return EDID_SUCCESS;
    }
    if(b0 == 0) {
        printf("%s: Reserved value in byte 0 of std timing offset 0x%x\n",
               __FUNCTION__, offset);
        return EDID_ERROR;
    }
    timing->h_active = b0*8+248; // VESA EEDID spec table 3.19
    switch(b1 & EDID_STANDARD_TIMINGS_AR_MASK) {
    case 0x00:
        timing->ar_numerator = 16;
        timing->ar_denominator = 10;
        break;
    case 0x40:
        timing->ar_numerator =  4;
        timing->ar_denominator =  3;
        break;
    case 0x80:
        timing->ar_numerator =  5;
        timing->ar_denominator =  4;
        break;
    case 0xc0:
        timing->ar_numerator = 16;
        timing->ar_denominator =  9;
        break;
    default:
        printf("%s: Something not right about AR field calculation\n", __FUNCTION__);
        return EDID_ERROR;
    }
    timing->v_active = timing->h_active * timing->ar_denominator / timing->ar_numerator;
    timing->v_frequency = (b1 & EDID_STANDARD_TIMINGS_FIELD_REFRESH_MASK) + 60;
    timing->internal_flags =
        EDID_TIMING_IFLAGS_AR_VALID |
        EDID_TIMING_IFLAGS_V_FREQ_VALID |
        EDID_TIMING_IFLAGS_ENTRY_VALID;
    return EDID_SUCCESS;
}

int cnc_edid_parse_standard_timings(cnc_edid_t *edid) {
    int i, ret=EDID_SUCCESS;
    cnc_edid_timing_t *timing;
    for(i=0; i<EDID_STANDARD_TIMINGS_COUNT; i++) {
        timing = cnc_edid_get_free_timing_struct(edid);
        if(timing == -1) {
            return EDID_ERROR;
        }
        ret |= cnc_edid_parse_and_store_standard_timing(edid, EDID_STANDARD_TIMINGS_BLOCK_OFFSET(i), timing);
        if(timing->internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
            timing->internal_flags |= EDID_TIMING_IFLAGS_LOCATION_STD;
        }
    }
#ifdef EDID_DEPRECATED
    memcpy(edid->edid_standard_timings,
           edid->edid_bin+EDID_STANDARD_TIMINGS_OFFSET,
           EDID_STANDARD_TIMINGS_LENGTH);
#endif
    edid->edid_valid |= EDID_VALID_STD_TIMINGS;
    EDID_DEBUG_DUMP(edid, EDID_STANDARD_TIMINGS_OFFSET, EDID_STANDARD_TIMINGS_LENGTH);
    return ret;
}

int cnc_edid_parse_display_descriptor(cnc_edid_t *edid, int block_offset) {
    int display_desc_type = edid->edid_bin[block_offset + EDID_DISPLAY_DESC_TAG_NUM_OFFSET];
    switch(display_desc_type) {
    case EDID_DISPLAY_DESC_DUMMY:
        break;
    case EDID_DISPLAY_DESC_DISPLAY_SERIAL_NUMBER: {
        char *name = (char *)edid->edid_bin + block_offset + EDID_DISPLAY_DESC_STRING_OFFSET;
        strncpy(edid->display_serial_number, name, EDID_DISPLAY_DESC_STRING_LENGTH);
        edid->display_serial_number[EDID_DISPLAY_DESC_STRING_LENGTH] = '\0';
        edid->edid_valid |= EDID_VALID_DISPLAY_SERIAL_NUMBER;
        break;
    }
    case EDID_DISPLAY_DESC_ALPHANUM_STRING: {
        char *name = (char *)edid->edid_bin + block_offset + EDID_DISPLAY_DESC_STRING_OFFSET;
        strncpy(edid->display_data_string, name, EDID_DISPLAY_DESC_STRING_LENGTH);
        edid->display_data_string[EDID_DISPLAY_DESC_STRING_LENGTH] = '\0';
        edid->edid_valid |= EDID_VALID_DISPLAY_DATA_STR;
        break;
    }
    case EDID_DISPLAY_DESC_DISPLAY_RANGE_LIMITS: {
        int flags = edid->edid_bin[block_offset + EDID_DISPLAY_RANGE_LIMITS_OFFSET];
        int v_min_add, v_max_add, h_min_add, h_max_add;
        switch(flags & EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MASK) {
        case 0:
            v_min_add = v_max_add = 0;
            break;
        case 2:
            v_min_add = 0;
            v_max_add = 255;
            break;
        case 3:
            v_min_add = v_max_add = 255;
            break;
        default:
            printf("%s: WARNING: reserved v_flags value in display range limits block at offset 0x%x\n", __FUNCTION__, block_offset);
            v_min_add = v_max_add = 0;
        }
        switch(flags & EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MASK) {
        case   0:
            h_min_add =    h_max_add =   0;
            break;
        case   8:
            h_min_add = 0;
            h_max_add = 255;
            break;
        case 0xc:
            h_min_add =    h_max_add = 255;
            break;
        default:
            printf("%s: WARNING: reserved h_flags value in display range limits block at offset 0x%x\n", __FUNCTION__, block_offset);
            h_min_add = h_max_add = 0;
        }
        edid->rate_v_min =
            edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_V_MIN_OFFSET] + v_min_add;
        edid->rate_v_max =
            edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_V_MAX_OFFSET] + v_max_add;
        edid->rate_h_min =
            edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_H_MIN_OFFSET] + h_min_add;
        edid->rate_h_max =
            edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_H_MAX_OFFSET] + h_max_add;
        edid->rate_pixel_clock_max =
            edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MAX_OFFSET] *
            EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MULTIPLIER;
        edid->edid_valid |= EDID_VALID_DISPLAY_RANGE_LIMITS;
        // FIXME: Byte 10 GTF support???
#ifdef EDID_DEPRECATED
        memcpy(edid->display_range_limits,
               edid->edid_bin + block_offset + EDID_DISPLAY_RANGE_LIMITS_OFFSET,
               EDID_DISPLAY_RANGE_LIMITS_COUNT);
#endif
        break;
    }
    case EDID_DISPLAY_DESC_DISPLAY_PRODUCT_NAME: {
        char *name = (char *)edid->edid_bin + block_offset + EDID_DISPLAY_DESC_STRING_OFFSET;
        strncpy(edid->display_product_name, name, EDID_DISPLAY_DESC_STRING_LENGTH);
        edid->display_product_name[EDID_DISPLAY_DESC_STRING_LENGTH] = '\0';
        edid->edid_valid |= EDID_VALID_DISPLAY_PRODUCT_NAME;
        break;
    }
    case EDID_DISPLAY_DESC_STD_TIMING_ID: {
        int i, ret=EDID_SUCCESS;
        cnc_edid_timing_t *timing;
        for(i=0; i<EDID_DISPLAY_DESC_STD_TIMINGS_COUNT; i++) {
            timing = cnc_edid_get_free_timing_struct(edid);
            if(timing == -1) {
                return EDID_ERROR;
            }
            ret |= cnc_edid_parse_and_store_standard_timing(edid, EDID_STANDARD_TIMINGS_BLOCK_OFFSET(i), timing);
            if(timing->internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
                timing->internal_flags |= EDID_TIMING_IFLAGS_LOCATION_EXT_18B_STD;
            }
        }
        break;
    }
    case EDID_DISPLAY_DESC_COLOR_POINT_DATA:
    case EDID_DISPLAY_DESC_DCM_DATA:
    case EDID_DISPLAY_DESC_CVT_3_BYTE_CODES:
    case EDID_DISPLAY_DESC_EST_TIMINGS_III:
        printf("%s: WARNING: Unparsed block type 0x%x\n", __FUNCTION__, display_desc_type);
        return EDID_ERROR;
    case 0: {
        int i, sum=0, bytes_remaining = (EDID_18B_BLOCK_LENGTH - EDID_DISPLAY_DESC_TAG_NUM_OFFSET);
        unsigned char *block = edid->edid_bin+block_offset + EDID_DISPLAY_DESC_TAG_NUM_OFFSET;
        for(i=1; i<bytes_remaining; i++) {
            sum += block[i];
        }
        if(sum == 0) {
            break;    // determined this 'block' is padding at the end of an extension block
        }
        // else fall through to the warning message in the default case
    }
    default:
        printf("%s: WARNING: Reserved or Mfg-specified display descriptor type 0x%02x\n", __FUNCTION__, display_desc_type);
        return EDID_ERROR;
    }
    return EDID_SUCCESS;
}

int cnc_edid_parse_dtd(cnc_edid_t *edid, int offset, cnc_edid_timing_t *timing) {
    unsigned char *block = edid->edid_bin + offset;
    if(timing == -1) {
        return EDID_ERROR;
    }
    timing->pixel_clock = (block[EDID_DTD_PIXEL_CLOCK_LO_OFFSET] |
                           (block[EDID_DTD_PIXEL_CLOCK_HI_OFFSET] << EDID_DTD_PIXEL_CLOCK_HI_SHIFT)) *
                          EDID_DTD_PIXEL_CLOCK_MULTIPLIER;
    timing->h_active = block[EDID_DTD_H_ACTIVE_LO_OFFSET] |
                       ((block[EDID_DTD_H_ACTIVE_HI_OFFSET] & EDID_DTD_H_ACTIVE_HI_MASK) << EDID_DTD_H_ACTIVE_HI_SHIFT);
    timing->v_active = block[EDID_DTD_V_ACTIVE_LO_OFFSET] |
                       ((block[EDID_DTD_V_ACTIVE_HI_OFFSET] & EDID_DTD_V_ACTIVE_HI_MASK) << EDID_DTD_V_ACTIVE_HI_SHIFT);
    timing->h_blank = block[EDID_DTD_H_BLANK_LO_OFFSET] |
                      ((block[EDID_DTD_H_BLANK_HI_OFFSET] & EDID_DTD_H_BLANK_HI_MASK) << EDID_DTD_H_BLANK_HI_SHIFT);
    timing->v_blank = block[EDID_DTD_V_BLANK_LO_OFFSET] |
                      ((block[EDID_DTD_V_BLANK_HI_OFFSET] & EDID_DTD_V_BLANK_HI_MASK) << EDID_DTD_V_BLANK_HI_SHIFT);
    timing->h_front_porch = block[EDID_DTD_H_FRONT_PORCH_LO_OFFSET] |
                            ((block[EDID_DTD_H_FRONT_PORCH_HI_OFFSET] & EDID_DTD_H_FRONT_PORCH_HI_MASK) << EDID_DTD_H_FRONT_PORCH_HI_SHIFT);
    timing->v_front_porch =
        ((block[EDID_DTD_V_FRONT_PORCH_LO_OFFSET] & EDID_DTD_V_FRONT_PORCH_LO_MASK) >> EDID_DTD_V_FRONT_PORCH_LO_SHIFT) |
        ((block[EDID_DTD_V_FRONT_PORCH_HI_OFFSET] & EDID_DTD_V_FRONT_PORCH_HI_MASK) << EDID_DTD_V_FRONT_PORCH_HI_SHIFT);
    timing->h_sync_width = block[EDID_DTD_H_SYNC_WIDTH_LO_OFFSET] |
                           ((block[EDID_DTD_H_SYNC_WIDTH_HI_OFFSET] & EDID_DTD_H_SYNC_WIDTH_HI_MASK) << EDID_DTD_H_SYNC_WIDTH_HI_SHIFT);
    timing->v_sync_width =
        ((block[EDID_DTD_V_SYNC_WIDTH_LO_OFFSET] & EDID_DTD_V_SYNC_WIDTH_LO_MASK) >>EDID_DTD_V_SYNC_WIDTH_LO_SHIFT) |
        ((block[EDID_DTD_V_SYNC_WIDTH_HI_OFFSET] & EDID_DTD_V_SYNC_WIDTH_HI_MASK) << EDID_DTD_V_SYNC_WIDTH_HI_SHIFT);
    timing->h_size_mm = block[EDID_DTD_H_SIZE_MM_LO_OFFSET] |
                        ((block[EDID_DTD_H_SIZE_MM_HI_OFFSET] & EDID_DTD_H_SIZE_MM_HI_MASK) << EDID_DTD_H_SIZE_MM_HI_SHIFT);
    timing->v_size_mm = block[EDID_DTD_V_SIZE_MM_LO_OFFSET] |
                        ((block[EDID_DTD_V_SIZE_MM_HI_OFFSET] & EDID_DTD_V_SIZE_MM_HI_MASK) << EDID_DTD_V_SIZE_MM_HI_SHIFT);
    timing->border_width = block[EDID_DTD_BORDER_WIDTH_OFFSET];
    timing->border_height = block[EDID_DTD_BORDER_HEIGHT_OFFSET];
    timing->timing_flags = block[EDID_DTD_FLAGS_OFFSET];
    timing->h_total = timing->h_active + timing->h_blank;
    timing->v_total = timing->v_active + timing->v_blank;
    timing->v_frequency = timing->pixel_clock / (timing->h_total * timing->v_total);
    timing->internal_flags =
        EDID_TIMING_IFLAGS_AR_VALID |
        EDID_TIMING_IFLAGS_V_FREQ_VALID |
        EDID_TIMING_IFLAGS_PIXEL_CLOCK_VALID |
        EDID_TIMING_IFLAGS_ENTRY_VALID;
    if((timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) != 0) {
        timing->v_active <<= 1;
    }    
    return EDID_SUCCESS;
}

int cnc_edid_parse_18_byte_block(cnc_edid_t *edid, int block_offset, cnc_edid_timing_t *timing) {
    int ret;
    if(edid->edid_bin[block_offset] == 0 &&
            edid->edid_bin[block_offset+1] == 0 &&
            edid->edid_bin[block_offset+2] == 0) {
        ret = cnc_edid_parse_display_descriptor(edid, block_offset);
    }
    else {
        ret = cnc_edid_parse_dtd(edid, block_offset, timing);
    }
    EDID_DEBUG_DUMP(edid, block_offset, EDID_18B_BLOCK_LENGTH);
    return ret;
}

// returns the number of extension blocks
int cnc_edid_parse_extension_blocks_count(cnc_edid_t *edid) {
    return edid->ext_blocks_count = edid->edid_bin[EDID_EXT_BLOCKS_COUNT];
}


int cnc_edid_parse_main_18_byte_blocks(cnc_edid_t *edid) {
    int i, offset, ret=EDID_SUCCESS;
    cnc_edid_timing_t *timing;
    for(i=0; i<4; i++) {
        offset = EDID_MAIN_18B_BLOCK_OFFSET(i);
        timing = cnc_edid_get_free_timing_struct(edid);
        if(timing == -1) {
            return EDID_ERROR;
        }
        ret |= cnc_edid_parse_18_byte_block(edid, offset, timing);
        if(ret == EDID_SUCCESS && timing->internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
            timing->internal_flags |= EDID_TIMING_IFLAGS_LOCATION_MAIN_18B;
            if(i == 0) {
                timing->internal_flags |= EDID_TIMING_IFLAGS_PREFERRED;
            }
        }
    }
    return ret;
}

// returns EDID_ERROR if any part fails or the number of extension blocks if it succeeds
int cnc_edid_parse_main(cnc_edid_t *edid) {
    int i, edid_retval=EDID_SUCCESS;
    EDID_VALIDATE_BINARY(edid, 0);
    edid_retval |= cnc_edid_check_sum(edid, 0); // main edid is block 0
    edid_retval |= cnc_edid_parse_header(edid);
    edid_retval |= cnc_edid_parse_vendor_product_id(edid);
    edid_retval |= cnc_edid_parse_version(edid);
    edid_retval |= cnc_edid_parse_display_params(edid);
    edid_retval |= cnc_edid_parse_chromaticity(edid);
    edid_retval |= cnc_edid_parse_established_timings(edid);
    edid_retval |= cnc_edid_parse_standard_timings(edid);
    edid_retval |= cnc_edid_parse_main_18_byte_blocks(edid);
    i = cnc_edid_parse_extension_blocks_count(edid);
    return edid_retval ? EDID_ERROR : i;
}

int cnc_edid_parse_eedid_data_block(cnc_edid_t *edid, int offset) {
    unsigned char *block = edid->edid_bin + offset;
    int tag = (block[0] & EDID_EXT_BLOCK_DATA_BLOCK_TAG_MASK) >>
              EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT;
    int block_length = block[EDID_EXT_BLOCK_DATA_BLOCK_LEN_OFFSET] &
                       EDID_EXT_BLOCK_DATA_BLOCK_LEN_MASK;
    int ret=EDID_SUCCESS;
    // tag types from Table 41 of CEA 861-E
    switch(tag) {
    case EDID_EXT_BLOCK_TYPE_AUDIO: {
        int i, n_sads = block_length/EDID_EXT_BLOCK_SAD_SIZE;
        cnc_edid_audio_t *audio_fmt;
        unsigned char *sad;
#ifdef EDID_DEBUG
        printf("%s: Audio byte 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
        if(block_length % EDID_EXT_BLOCK_SAD_SIZE == 0) {
            printf("\t%d SADs in this block\n", n_sads);
        }
#endif
        if(block_length % EDID_EXT_BLOCK_SAD_SIZE) {
            printf("\tweird Audio block len %d\n", block_length);
            return EDID_ERROR;
        }
        for(i=0; i<n_sads; i++) {
            int format, num_channels;
            sad = block + 1 + (i*EDID_EXT_BLOCK_SAD_SIZE);
            format = (sad[0] & EDID_EXT_BLOCK_SAD_FORMAT_CODE_MASK) >> EDID_EXT_BLOCK_SAD_FORMAT_CODE_SHIFT;
            num_channels = sad[0] & EDID_EXT_BLOCK_SAD_NUM_CHANNELS_MASK;
            if(format == EDID_AUDIO_FORMAT_RESERVED) {
                printf("%s: Audio format in SAD %d is reserved value (0)\n", __FUNCTION__, i);
                continue;
            }
            audio_fmt = cnc_edid_get_free_audio_struct(edid);
            if(audio_fmt == -1) {
                return EDID_ERROR;
            }
            audio_fmt->format = format;
            audio_fmt->max_channels = num_channels+1;
            audio_fmt->frequencies = sad[1];
            audio_fmt->extra_data = sad[2];
#ifdef EDID_DEBUG
            printf("\tformat %d, frequencies:0x%02x, extra:%02x\n", format, sad[1], sad[2]);
#endif
        }
        edid->edid_valid |= EDID_VALID_AUDIO_FORMATS;
        break;
    }
    case EDID_EXT_BLOCK_TYPE_VIDEO: {
        cnc_edid_timing_t *timing;
        int i;
        for(i=1; i<=block_length; i++) {
            timing = cnc_edid_get_free_timing_struct(edid);
            if(timing == -1) {
                return EDID_ERROR;
            }
            ret |= cnc_edid_vic_to_edid_timing(timing,
                                           block[i] & EDID_EXT_BLOCK_TYPE_VIDEO_VIC_MASK);
            if(timing->internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
                if(block[i] & EDID_EXT_BLOCK_TYPE_VIDEO_NATIVE_MASK) {
                    timing->internal_flags |= EDID_TIMING_IFLAGS_NATIVE;
                }
                timing->internal_flags |= EDID_TIMING_IFLAGS_LOCATION_SVD;
            }
        }
#ifdef EDID_DEBUG
        printf("%s: Video byte 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
#endif
        edid->edid_valid |= EDID_VALID_SVDS_PRESENT;
        break;
    }
    case EDID_EXT_BLOCK_TYPE_VENDOR_SPECIFIC: {
        int i;
        for(i=0; i<block_length; i++) {
            edid->vendor_specific[i] = block[i+1];
        }
        edid->vendor_specific_length = block_length;
#ifdef EDID_DEBUG
        printf("%s: Vendor byte 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
#endif
        edid->edid_valid |= EDID_VALID_VENDOR_SPECIFIC;
        break;
    }
    case EDID_EXT_BLOCK_TYPE_SPEAKER:
        if(block[0] != 0x83) {
            printf("%s: Speaker Alloc header is weird: 0x%02x offset 0x%02x\n",  __FUNCTION__, block[0], offset);
            break;
        }
        edid->speaker_alloc = block[1] | block[2]<<EDID_SPEAKER_BYTE2_SHIFT;
#ifdef EDID_DEBUG
        printf("%s: Speaker byte 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
#endif
        edid->edid_valid |= EDID_VALID_SPEAKER_ALLOC;
        break;
    case EDID_EXT_BLOCK_TYPE_VESA_DTC:
#ifdef EDID_DEBUG
        printf("%s: VESA DTC byte 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
#endif
        break;
    case EDID_EXT_BLOCK_TYPE_EXTENDED: { // extended tag
        int etag = block[1];
        if(etag == EDID_EXTENDED_TAG_CODE_VIDCAP) {
            edid->video_capability = block[2];
            edid->edid_valid |= EDID_VALID_VIDEO_CAPABILITY;
        }
        else if(etag == EDID_EXTENDED_TAG_CODE_COLORIMETRY) {
            edid->colorimetry = block[2] | (block[3] << EDID_COLORIMETRY_BYTE2_SHIFT);
            edid->edid_valid |= EDID_VALID_COLORIMETRY;
        }
        else {
            printf("%s: Unhandled Extended Type Tag 0x%02x offset 0x%x\n", __FUNCTION__, etag, offset);
        }
#ifdef EDID_DEBUG
        if(etag == EDID_EXTENDED_TAG_CODE_VIDCAP) {
            printf("%s: VidCap Extended Type Tag offset 0x%x\n", __FUNCTION__, offset);
        }
        else if(etag == EDID_EXTENDED_TAG_CODE_COLORIMETRY) {
            printf("%s: Colorimetry Extended Type Tag offset 0x%x\n", __FUNCTION__, offset);
        }
#endif
        break;
    }
    default:
        printf("%s: Reserved Type Tag 0x%02x offset 0x%x\n", __FUNCTION__, block[0], offset);
    }
    return ret;
}

int cnc_edid_parse_extension(cnc_edid_t *edid, int which_block) {
    int ret, offset, dtd_start_offset, dtd_count, parsed_block_len;
    int block_offset = EDID_EXT_BLOCK_OFFSET(which_block);
    unsigned char *block = edid->edid_bin + block_offset;
    cnc_edid_timing_t *timing;
    ret = cnc_edid_check_sum(edid, which_block);
    if(block[EDID_EXT_BLOCK_TAG_OFFSET] != EDID_EXT_BLOCK_TAG_EXPECTED_VALUE) {
        printf("%s: ERROR: EEDID Block Tag unexpected value 0x%02x, offset 0x%x\n",
               __FUNCTION__,
               block[EDID_EXT_BLOCK_TAG_OFFSET], EDID_EXT_BLOCK_OFFSET(which_block));
        return EDID_ERROR;
    }
    if(block[EDID_EXT_BLOCK_REVISION_OFFSET] != EDID_EXT_BLOCK_REVISION_EXPECTED_VALUE) {
        printf("%s: WARNING: EEDID Block Revision unexpected value 0x%02x, offset 0x%x\n",
               __FUNCTION__,
               block[EDID_EXT_BLOCK_TAG_OFFSET], EDID_EXT_BLOCK_OFFSET(which_block));
    }
    // dtd_start_offset and the loop control variable offset are relative to this block
    dtd_start_offset = block[EDID_EXT_BLOCK_18B_BLOCK_START_OFFSET];
    edid->eedid_flags = block[EDID_EXT_BLOCK_FLAGS_OFFSET];
    dtd_count = edid->eedid_flags & EDID_EXT_BLOCK_FLAGS_DTD_COUNT_MASK;
    for(offset = EDID_EXT_BLOCK_DATA_BLOCKS_OFFSET;
            offset < dtd_start_offset;
            offset += parsed_block_len) {
        ret |= cnc_edid_parse_eedid_data_block(edid, EDID_EXT_BLOCK_OFFSET(which_block) + offset);
        parsed_block_len = (block[offset] & EDID_EXT_BLOCK_DATA_BLOCK_LEN_MASK) + 1;
    }
    // This should maybe be fatal.  Bad juju would occur if alignment was off.
    if(offset != dtd_start_offset)
        printf("%s: WARNING: end of data blocks != start of 18B blocks (0x%x != 0x%x)\n",
               __FUNCTION__, offset, dtd_start_offset);
    for(; offset<(EDID_EXT_BLOCK_SIZE-EDID_18B_BLOCK_LENGTH); offset+=EDID_18B_BLOCK_LENGTH) {
        timing = cnc_edid_get_free_timing_struct(edid);
        if(timing == -1) {
            return EDID_ERROR;
        }
        ret |= cnc_edid_parse_18_byte_block(edid, block_offset + offset, timing);
        if(timing->internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
            timing->internal_flags |= EDID_TIMING_IFLAGS_LOCATION_EXT_18B;
        }
    }
    for(; offset<EDID_CHECKSUM_OFFSET; offset++) {
        if(block[offset] != 0) {
            printf("%s: WARNING: wanted padding, found non-zero data at offset 0x%x, eedid blcok %d\n", __FUNCTION__, offset, which_block);
        }
    }
    edid->edid_valid |= EDID_VALID_EEDID; // sufficient?
    return ret;
}

int cnc_edid_parse(cnc_edid_t *edid) {
    int i, n_ext_blocks, ret=EDID_SUCCESS;
    n_ext_blocks = cnc_edid_parse_main(edid);
    if(n_ext_blocks <= 0) {
        printf("%s: not parsing eedid %d\n", __FUNCTION__, n_ext_blocks);
        return n_ext_blocks; // failed or no ext blocks
    }
    for(i=1; i<=n_ext_blocks; i++) {
        ret |= cnc_edid_parse_extension(edid, i);
    }
    return ret;
}

