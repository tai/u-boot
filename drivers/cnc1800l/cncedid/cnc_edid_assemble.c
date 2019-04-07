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

// Not checking edid_valid anywhere in here yet.
// Philosophical question: should bounds check here or in parser?  It's not here now.

int cnc_edid_assemble_header(cnc_edid_t *edid) {
    unsigned char header[EDID_HEADER_LENGTH] = EDID_HEADER_INITIALIZER;
    memcpy(edid->edid_bin, header, EDID_HEADER_LENGTH);
    return EDID_SUCCESS;
}

int cnc_edid_assemble_vendor_product_id(cnc_edid_t *edid) {
    unsigned char byte0, byte1, tmp;
    byte0 = (edid->id_mfg_name[0] - '@') << 2;
    byte0 &= 0x7c;
    tmp = edid->id_mfg_name[1] - '@';
    byte0 |= (tmp & 0x18) >> 3;
    byte1 = (tmp & 7) << 5;
    byte1 |= edid->id_mfg_name[2] - '@';
    edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+0] = byte0;
    edid->edid_bin[EDID_ID_MFG_NAME_OFFSET+1] = byte1;
    EDID_ASSEMBLE_INT_TO_SHORT(edid->edid_bin+EDID_ID_PRODUCT_CODE_OFFSET, edid->id_product_code);
    EDID_ASSEMBLE_INT_TO_LONG(edid->edid_bin+EDID_ID_SERIAL_NUMBER_OFFSET, edid->id_serial_number);
    edid->edid_bin[EDID_WEEK_OF_MFG_OFFSET] = edid->week_of_mfg;
    edid->edid_bin[EDID_YEAR_OF_MFG_OFFSET] = edid->year_of_mfg;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_version(cnc_edid_t *edid) {
    edid->edid_bin[EDID_VERSION_OFFSET] = edid->version;
    edid->edid_bin[EDID_REVISION_OFFSET] = edid->revision;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_display_params(cnc_edid_t *edid) {
    if(edid->signal_interface == EDID_VIDEO_INPUT_DEF_SIGNAL_INT_DIGITAL) {
        edid->edid_bin[EDID_VIDEO_INPUT_DEF_OFFSET] = EDID_VIDEO_INPUT_DEF_SIGNAL_INT_MASK |
                (edid->color_bit_depth << EDID_VIDEO_INPUT_DEF_COLOR_DEPTH_SHIFT) |
                edid->interface_std;
    }
    else {   // EDID_VIDEO_INPUT_DEF_SIGNAL_INT_ANALOG
        edid->edid_bin[EDID_DISPLAY_PARAMS_OFFSET] = edid->video_input_def;
    }
    edid->edid_bin[EDID_VIDEO_INPUT_HORIZONTAL_OFFSET] = edid->horizontal;
    edid->edid_bin[EDID_VIDEO_INPUT_VERTICAL_OFFSET] = edid->vertical;
    edid->edid_bin[EDID_VIDEO_INPUT_GAMMA_OFFSET] = edid->gamma;
    edid->edid_bin[EDID_VIDEO_INPUT_FEATURE_SUPPORT_OFFSET] = edid->feature_support;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_chromaticity(cnc_edid_t *edid) {
    int lo_0, lo_1;
    lo_0 = ((edid->chromaticity[EDID_CHROMATICITY_Rx_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_RxLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_Ry_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_RyLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_Gx_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_GxLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_Gy_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_GyLo_SHIFT);
    lo_1 = ((edid->chromaticity[EDID_CHROMATICITY_Bx_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_BxLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_By_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_ByLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_Wx_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_WxLo_SHIFT) |
           ((edid->chromaticity[EDID_CHROMATICITY_Wy_INDEX] & EDID_CHROMATICITY_LoByte_MASK) << EDID_CHROMATICITY_WyLo_SHIFT);
    edid->edid_bin[EDID_CHROMATICITY_RxLo_OFFSET] = lo_0;
    edid->edid_bin[EDID_CHROMATICITY_BxLo_OFFSET] = lo_1;
    edid->edid_bin[EDID_CHROMATICITY_RxHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Rx_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_RyHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Ry_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_GxHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Gx_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_GyHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Gy_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_BxHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Bx_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_ByHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_By_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_WxHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Wx_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    edid->edid_bin[EDID_CHROMATICITY_WyHi_OFFSET] = edid->chromaticity[EDID_CHROMATICITY_Wy_INDEX] >> EDID_CHROMATICITY_HiByte_SHIFT;
    return EDID_SUCCESS;
}

// return true if the resolution is found in the list
int edid_search_established_timing(cnc_edid_t *edid, int h, int v, int hz, int interlaced) {
    int i;
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if(edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_ENTRY_VALID) {
            cnc_edid_timing_t *timing = &(edid->timings[i]);
            if(timing->h_active == h &&
                    timing->v_active == v &&
                    timing->v_frequency == hz) {
                if(timing->timing_flags & EDID_DTD_FLAGS_INTERLACED) {
                    return interlaced ? 1 : 0;
                }
                else {
                    return interlaced ? 0 : 1;
                }
            }
        }
    return 0;
}

int cnc_edid_assemble_established_timings(cnc_edid_t *edid) {
    edid->edid_bin[EDID_ESTABLISHED_TIMINGS_I_OFFSET] =
        (edid_search_established_timing(edid, 720, 400, 70, 0)   ? 0x80 : 0) |
        (edid_search_established_timing(edid, 720, 400, 88, 0)   ? 0x40 : 0) |
        (edid_search_established_timing(edid, 640, 480, 60, 0)   ? 0x20 : 0) |
        (edid_search_established_timing(edid, 640, 480, 67, 0)   ? 0x10 : 0) |
        (edid_search_established_timing(edid, 640, 480, 72, 0)   ? 8 : 0) |
        (edid_search_established_timing(edid, 640, 480, 75, 0)   ? 4 : 0) |
        (edid_search_established_timing(edid, 800, 600, 56, 0)   ? 2 : 0) |
        (edid_search_established_timing(edid, 800, 600, 60, 0)   ? 1 : 0);
    edid->edid_bin[EDID_ESTABLISHED_TIMINGS_II_OFFSET] =
        (edid_search_established_timing(edid, 800, 600, 72, 0)   ? 0x80 : 0) |
        (edid_search_established_timing(edid, 800, 600, 75, 0)   ? 0x40 : 0) |
        (edid_search_established_timing(edid, 832, 624, 75, 0)   ? 0x20 : 0) |
        (edid_search_established_timing(edid, 1024, 768, 87, 1)  ? 0x10 : 0) |
        (edid_search_established_timing(edid, 1024, 768, 60, 0)  ? 8 : 0) |
        (edid_search_established_timing(edid, 1024, 768, 70, 0)  ? 4 : 0) |
        (edid_search_established_timing(edid, 1024, 768, 75, 0)  ? 2 : 0) |
        (edid_search_established_timing(edid, 1280, 1024, 75, 0) ? 1 : 0);
    edid->edid_bin[EDID_ESTABLISHED_TIMINGS_MFG_OFFSET] =
        edid_search_established_timing(edid, 1152, 870, 75, 0)  ? 0x80 : 0;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_standard_timings(cnc_edid_t *edid) {
    int i;
    int std_timings_count=0;
    unsigned char *b0 = edid->edid_bin+EDID_STANDARD_TIMINGS_OFFSET;
    unsigned char *b1 = b0+1;
    int flags = (EDID_TIMING_IFLAGS_ENTRY_VALID |
                 EDID_TIMING_IFLAGS_AR_VALID |
                 EDID_TIMING_IFLAGS_V_FREQ_VALID |
                 EDID_TIMING_IFLAGS_LOCATION_STD);
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++) {
        if((edid->timings[i].internal_flags & flags) == flags) {
            int ar;
            cnc_edid_timing_t *timing = &(edid->timings[i]);
            if(timing->ar_numerator == 16 && timing->ar_denominator == 10) {
                ar = 0;
            }
            else if(timing->ar_numerator == 4 && timing->ar_denominator == 3) {
                ar = 0x40;
            }
            else if(timing->ar_numerator == 5 && timing->ar_denominator == 4) {
                ar = 0x80;
            }
            else if(timing->ar_numerator == 16 && timing->ar_denominator == 9) {
                ar = 0xc0;
            }
            else {
                printf("%s: weird Aspect Ratio found (%d:%d), skipping\n", __FUNCTION__, timing->ar_numerator, timing->ar_denominator);
                continue;
            }
            *b0 = (timing->h_active - 248) / 8; // VESA EEDID spec table 3.19
            *b1 = ar | ((timing->v_frequency - 60) & EDID_STANDARD_TIMINGS_FIELD_REFRESH_MASK);
            b0 += EDID_STANDARD_TIMINGS_LENGTH_EACH;
            b1 += EDID_STANDARD_TIMINGS_LENGTH_EACH;
            if(++std_timings_count >= EDID_STANDARD_TIMINGS_COUNT) {
                break;
            }
        }
    }
    for(; std_timings_count < EDID_STANDARD_TIMINGS_COUNT; std_timings_count++) {
        *b0 = *b1 = 1;
        b0 += EDID_STANDARD_TIMINGS_LENGTH_EACH;
        b1 += EDID_STANDARD_TIMINGS_LENGTH_EACH;
    }
    return EDID_SUCCESS;
}

int cnc_edid_assemble_dtd(cnc_edid_t *edid, int offset, int timing_index) {
    unsigned char *block = edid->edid_bin + offset;
    cnc_edid_timing_t *timing = &(edid->timings[timing_index]);
    int pixclk_scaled = timing->pixel_clock / EDID_DTD_PIXEL_CLOCK_MULTIPLIER;
    block[EDID_DTD_PIXEL_CLOCK_LO_OFFSET] = pixclk_scaled & 0xff;
    block[EDID_DTD_PIXEL_CLOCK_HI_OFFSET] = pixclk_scaled >> 8;
    block[EDID_DTD_H_ACTIVE_LO_OFFSET] = timing->h_active & EDID_DTD_LO_MASK;
    block[EDID_DTD_H_BLANK_LO_OFFSET] = timing->h_blank & EDID_DTD_LO_MASK;
    block[EDID_DTD_H_ACTIVE_HI_OFFSET] =
        ((timing->h_active >> EDID_DTD_H_ACTIVE_HI_SHIFT) & EDID_DTD_H_ACTIVE_HI_MASK) |
        ((timing->h_blank  >> EDID_DTD_H_BLANK_HI_SHIFT) & EDID_DTD_H_BLANK_HI_MASK);
    block[EDID_DTD_V_ACTIVE_LO_OFFSET] = timing->v_active & EDID_DTD_LO_MASK;
    block[EDID_DTD_V_BLANK_LO_OFFSET] = timing->v_blank & EDID_DTD_LO_MASK;
    block[EDID_DTD_V_ACTIVE_HI_OFFSET] =
        ((timing->v_active >> EDID_DTD_V_ACTIVE_HI_SHIFT) & EDID_DTD_V_ACTIVE_HI_MASK) |
        ((timing->v_blank >> EDID_DTD_V_BLANK_HI_SHIFT) & EDID_DTD_V_BLANK_HI_MASK);
    block[EDID_DTD_H_FRONT_PORCH_LO_OFFSET] = timing->h_front_porch & EDID_DTD_LO_MASK;
    block[EDID_DTD_H_SYNC_WIDTH_LO_OFFSET] = timing->h_sync_width & EDID_DTD_LO_MASK;
    block[EDID_DTD_V_FRONT_PORCH_LO_OFFSET] =
        ((timing->v_front_porch << EDID_DTD_V_FRONT_PORCH_LO_SHIFT) & EDID_DTD_V_FRONT_PORCH_LO_MASK) |
        ((timing->v_sync_width << EDID_DTD_V_SYNC_WIDTH_LO_SHIFT) & EDID_DTD_V_SYNC_WIDTH_LO_MASK);
    block[EDID_DTD_H_FRONT_PORCH_HI_OFFSET] =
        ((timing->h_front_porch >> EDID_DTD_H_FRONT_PORCH_HI_SHIFT) & EDID_DTD_H_FRONT_PORCH_HI_MASK) |
        ((timing->h_sync_width >> EDID_DTD_H_SYNC_WIDTH_HI_SHIFT) & EDID_DTD_H_SYNC_WIDTH_HI_MASK) |
        ((timing->v_front_porch >> EDID_DTD_V_FRONT_PORCH_HI_SHIFT) & EDID_DTD_V_FRONT_PORCH_HI_MASK) |
        ((timing->v_sync_width >> EDID_DTD_V_SYNC_WIDTH_HI_SHIFT) & EDID_DTD_V_SYNC_WIDTH_HI_MASK);
    block[EDID_DTD_H_SIZE_MM_LO_OFFSET] = timing->h_size_mm & EDID_DTD_LO_MASK;
    block[EDID_DTD_V_SIZE_MM_LO_OFFSET] = timing->v_size_mm & EDID_DTD_LO_MASK;
    block[EDID_DTD_H_SIZE_MM_HI_OFFSET] =
        ((timing->h_size_mm >> EDID_DTD_H_SIZE_MM_HI_SHIFT) & EDID_DTD_H_SIZE_MM_HI_MASK) |
        ((timing->v_size_mm >> EDID_DTD_V_SIZE_MM_HI_SHIFT) & EDID_DTD_V_SIZE_MM_HI_MASK);
    block[EDID_DTD_BORDER_WIDTH_OFFSET] = timing->border_width;
    block[EDID_DTD_BORDER_HEIGHT_OFFSET] = timing->border_height;
    block[EDID_DTD_FLAGS_OFFSET] = timing->timing_flags;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_display_descriptor(cnc_edid_t *edid, int block_offset, int display_desc_type) {
    int i;
    switch(display_desc_type) {
    case EDID_DISPLAY_DESC_DUMMY: {
        for(i=0; i<EDID_18B_BLOCK_LENGTH; i++) {
            edid->edid_bin[block_offset++] = (i == 3) ? EDID_DISPLAY_DESC_DUMMY : 0;
        }
        break;
    }
    case EDID_DISPLAY_DESC_DISPLAY_PRODUCT_NAME: {
        char *name = (char *)edid->edid_bin + block_offset + EDID_DISPLAY_DESC_STRING_OFFSET;
        for(i=0; i<3; i++) {
            edid->edid_bin[block_offset++] = 0;
        }
        edid->edid_bin[block_offset++] = EDID_DISPLAY_DESC_DISPLAY_PRODUCT_NAME;
        strncpy(name, edid->display_product_name, EDID_DISPLAY_DESC_STRING_LENGTH);
        break;
    }
    case EDID_DISPLAY_DESC_DISPLAY_RANGE_LIMITS: {
        int rate_v_min, rate_v_max, rate_h_min, rate_h_max;
        int rate_v_min_shift=0, rate_v_max_shift=0, rate_h_min_shift=0, rate_h_max_shift=0;
        int offset = block_offset;
        for(i=0; i<3; i++) {
            edid->edid_bin[offset++] = 0;
        }
        edid->edid_bin[offset] = EDID_DISPLAY_DESC_DISPLAY_RANGE_LIMITS;
        rate_v_min = edid->rate_v_min;
        if(rate_v_min > 255) {
            rate_v_min -= 255;
            rate_v_min_shift = EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MIN;
        }
        rate_v_max = edid->rate_v_max;
        if(rate_v_max > 255) {
            rate_v_max -= 255;
            rate_v_max_shift = EDID_DISPLAY_RANGE_LIMITS_FLAGS_V_MAX;
        }
        rate_h_min = edid->rate_h_min;
        if(rate_h_min > 255) {
            rate_h_min -= 255;
            rate_h_min_shift = EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MIN;
        }
        rate_h_max = edid->rate_h_max;
        if(rate_h_max > 255) {
            rate_h_max -= 255;
            rate_h_max_shift = EDID_DISPLAY_RANGE_LIMITS_FLAGS_H_MAX;
        }
        edid->edid_bin[block_offset + EDID_DISPLAY_RANGE_LIMITS_OFFSET] =
            rate_v_min_shift | rate_v_max_shift | rate_h_min_shift | rate_h_max_shift;
        edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_V_MIN_OFFSET] = rate_v_min;
        edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_V_MAX_OFFSET] = rate_v_max;
        edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_H_MIN_OFFSET] = rate_h_min;
        edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_H_MAX_OFFSET] = rate_h_max;
        edid->edid_bin[block_offset+EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MAX_OFFSET] =
            edid->rate_pixel_clock_max / EDID_DISPLAY_RANGE_LIMITS_PIXCLK_MULTIPLIER;
        // FIXME: Byte 10 GTF support???
        offset = EDID_DISPLAY_RANGE_LIMITS_GTF_OFFSET;
        edid->edid_bin[block_offset+offset++] = 0;
        edid->edid_bin[block_offset+offset++] = 0xa;
        while(offset<EDID_18B_BLOCK_LENGTH) {
            edid->edid_bin[block_offset+offset++] = 0x20;
        }
        break;
    }
    case EDID_DISPLAY_DESC_DISPLAY_SERIAL_NUMBER: {
        char *name = (char *)edid->edid_bin + block_offset + EDID_DISPLAY_DESC_STRING_OFFSET;
        for(i=0; i<3; i++) {
            edid->edid_bin[block_offset++] = 0;
        }
        edid->edid_bin[block_offset++] = EDID_DISPLAY_DESC_DISPLAY_SERIAL_NUMBER;
        strncpy(name, edid->display_serial_number, EDID_DISPLAY_DESC_STRING_LENGTH);
        break;
    }
    default:
        printf("%s: WARNING: Reserved or Mfg-specified display descriptor type 0x%02x\n", __FUNCTION__, display_desc_type);
        return EDID_ERROR;
    }
    return EDID_SUCCESS;
}

int cnc_edid_assemble_main_18_byte_blocks(cnc_edid_t *edid) {
    int i, s=0; // s is a counter of 18 byte blocks that have been filled
    int flags = (EDID_TIMING_IFLAGS_ENTRY_VALID |
                 EDID_TIMING_IFLAGS_AR_VALID |
                 EDID_TIMING_IFLAGS_V_FREQ_VALID |
                 EDID_TIMING_IFLAGS_LOCATION_MAIN_18B);
    int save_timing_indices[2];
    unsigned char offset = EDID_MAIN_18B_OFFSET;
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if((edid->timings[i].internal_flags & flags) == flags) {
            save_timing_indices[s++] = i;
            if(s == 2) {
                break;
            }
        }
    for(i=0; i<s; i++) {
        cnc_edid_assemble_dtd(edid, offset, save_timing_indices[i]);
        offset += EDID_18B_BLOCK_LENGTH;
    }
    if(edid->edid_valid | EDID_VALID_DISPLAY_PRODUCT_NAME) {
        cnc_edid_assemble_display_descriptor(edid, offset, EDID_DISPLAY_DESC_DISPLAY_PRODUCT_NAME);
        offset += EDID_18B_BLOCK_LENGTH;
        s++;
    }
    if(edid->edid_valid | EDID_VALID_DISPLAY_RANGE_LIMITS) {
        cnc_edid_assemble_display_descriptor(edid, offset, EDID_DISPLAY_DESC_DISPLAY_RANGE_LIMITS);
        offset += EDID_18B_BLOCK_LENGTH;
        s++;
    }
    if(s < 4 && edid->edid_valid | EDID_VALID_DISPLAY_SERIAL_NUMBER) {
        cnc_edid_assemble_display_descriptor(edid, offset, EDID_DISPLAY_DESC_DISPLAY_SERIAL_NUMBER);
        offset += EDID_18B_BLOCK_LENGTH;
        s++;
    }
    while(s<4) {
        cnc_edid_assemble_display_descriptor(edid, offset, EDID_DISPLAY_DESC_DUMMY);
        offset += EDID_18B_BLOCK_LENGTH;
        s++;
    }
    return EDID_SUCCESS;
}

// responsible for setting edid->ext_blocks_count and setting its valid bit
int cnc_edid_assemble_extension_blocks_count(cnc_edid_t *edid) {
    edid->edid_bin[EDID_EXT_BLOCKS_COUNT] = edid->ext_blocks_count;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_checksum(cnc_edid_t *edid, int block_num) {
    int i, checksum = 0;
    unsigned char *block_p = edid->edid_bin+EDID_EXT_BLOCK_OFFSET(block_num);
    block_p[EDID_CHECKSUM_OFFSET] = 0;
    for(i=0; i<EDID_BLOCK_LENGTH; i++) {
        checksum += block_p[i];
    }
    block_p[EDID_CHECKSUM_OFFSET] = 0x100-checksum;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_main(cnc_edid_t *edid) {
    int ret;
    ret  = cnc_edid_assemble_header(edid);
    ret |= cnc_edid_assemble_vendor_product_id(edid);
    ret |= cnc_edid_assemble_version(edid);
    ret |= cnc_edid_assemble_display_params(edid);
    ret |= cnc_edid_assemble_chromaticity(edid);
    ret |= cnc_edid_assemble_established_timings(edid);
    ret |= cnc_edid_assemble_standard_timings(edid);
    ret |= cnc_edid_assemble_main_18_byte_blocks(edid);
    ret |= cnc_edid_assemble_extension_blocks_count(edid);
    ret |= cnc_edid_assemble_checksum(edid, 0); // block 0
    if(ret == EDID_SUCCESS) {
        ret = edid->ext_blocks_count;
    }
    return ret;
}

int cnc_edid_assemble_ext_video_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    int i;
    int flags = (EDID_TIMING_IFLAGS_ENTRY_VALID |
                 EDID_TIMING_IFLAGS_LOCATION_SVD);
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if((edid->timings[i].internal_flags & flags) == flags) {
            *current_p++ = edid->timings[i].vic |
                           ((edid->timings[i].internal_flags & EDID_TIMING_IFLAGS_NATIVE) ?
                            EDID_EXT_BLOCK_TYPE_VIDEO_NATIVE_MASK : 0);
        }
    *block_p =
        (EDID_EXT_BLOCK_TYPE_VIDEO << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_audio_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    int i;
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    for(i=0; i<EDID_AUDIO_STORAGE_SIZE; i++)
        if(edid->audio_formats[i].format != EDID_AUDIO_FORMAT_RESERVED) {
            *current_p++ =
                (edid->audio_formats[i].format << EDID_EXT_BLOCK_SAD_FORMAT_CODE_SHIFT) |
                (edid->audio_formats[i].max_channels-1);
            *current_p++ = edid->audio_formats[i].frequencies;
            *current_p++ = edid->audio_formats[i].extra_data;
        }
    *block_p =
        (EDID_EXT_BLOCK_TYPE_AUDIO << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_speaker_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    *current_p++ = edid->speaker_alloc & EDID_SPEAKER_BYTE1_MASK;
    *current_p++ = edid->speaker_alloc >> EDID_SPEAKER_BYTE2_SHIFT;
    *current_p++ = EDID_SPEAKER_BYTE3_VALUE;
    *block_p =
        (EDID_EXT_BLOCK_TYPE_SPEAKER << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_vendor_specific_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    int i;
    for(i=0; i<edid->vendor_specific_length; i++) {
        *current_p++ = edid->vendor_specific[i];
    }
    *block_p =
        (EDID_EXT_BLOCK_TYPE_VENDOR_SPECIFIC << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_vidcap_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    *current_p++ = EDID_EXT_BLOCK_TYPE_EXTENDED_VIDCAP;
    *current_p++ = edid->video_capability;
    *block_p =
        (EDID_EXT_BLOCK_TYPE_EXTENDED << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_colorimetry_block(cnc_edid_t *edid, unsigned char **assemble_p) {
    unsigned char *block_p = *assemble_p;
    unsigned char *current_p = block_p+1;
    *current_p++ = EDID_EXT_BLOCK_TYPE_EXTENDED_COLORIMETRY;
    *current_p++ = edid->colorimetry & EDID_COLORIMETRY_BYTE1_MASK;
    *current_p++ = edid->colorimetry >> EDID_COLORIMETRY_BYTE2_SHIFT;
    *block_p =
        (EDID_EXT_BLOCK_TYPE_EXTENDED << EDID_EXT_BLOCK_DATA_BLOCK_TAG_SHIFT) |
        ((current_p - block_p)-1);
    *assemble_p = current_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_ext_18b_blocks(cnc_edid_t *edid, unsigned char **assemble_p) {
    int i;
    int flags = (EDID_TIMING_IFLAGS_ENTRY_VALID |
                 EDID_TIMING_IFLAGS_LOCATION_EXT_18B);
    unsigned char *block_p = *assemble_p;
    for(i=0; i<EDID_TIMINGS_STORAGE_SIZE; i++)
        if((edid->timings[i].internal_flags & flags) == flags) {
            cnc_edid_assemble_dtd(edid, block_p-edid->edid_bin, i);
            block_p += EDID_18B_BLOCK_LENGTH;
        }
    *assemble_p = block_p;
    return EDID_SUCCESS;
}

int cnc_edid_assemble_extensions(cnc_edid_t *edid) {
    int ret=EDID_SUCCESS;
    unsigned char *block_p = edid->edid_bin+EDID_EXT_BLOCK_OFFSET(1);
    unsigned char *current_p = block_p+EDID_EXT_BLOCK_DATA_BLOCKS_OFFSET;
    block_p[EDID_EXT_BLOCK_TAG_OFFSET] = EDID_EXT_BLOCK_TAG_EXPECTED_VALUE;
    block_p[EDID_EXT_BLOCK_REVISION_OFFSET] = EDID_EXT_BLOCK_REVISION_EXPECTED_VALUE;
    // FIXME: make sure the native-formats counter within this field is updated
    block_p[EDID_EXT_BLOCK_FLAGS_OFFSET] = edid->eedid_flags;
    if(edid->edid_valid & EDID_VALID_SVDS_PRESENT) {
        cnc_edid_assemble_ext_video_block(edid, &current_p);
    }
    if(edid->edid_valid & EDID_VALID_AUDIO_FORMATS) {
        cnc_edid_assemble_ext_audio_block(edid, &current_p);
    }
    if(edid->edid_valid & EDID_VALID_SPEAKER_ALLOC) {
        cnc_edid_assemble_ext_speaker_block(edid, &current_p);
    }
    if(edid->edid_valid & EDID_VALID_VENDOR_SPECIFIC) {
        cnc_edid_assemble_ext_vendor_specific_block(edid, &current_p);
    }
    if(edid->edid_valid & EDID_VALID_VIDEO_CAPABILITY) {
        cnc_edid_assemble_ext_vidcap_block(edid, &current_p);
    }
    if(edid->edid_valid & EDID_VALID_COLORIMETRY) {
        cnc_edid_assemble_ext_colorimetry_block(edid, &current_p);
    }
    block_p[EDID_EXT_BLOCK_18B_BLOCK_START_OFFSET] = current_p - block_p;
    cnc_edid_assemble_ext_18b_blocks(edid, &current_p);
    while((current_p - block_p) < EDID_EXT_BLOCK_SIZE) {
        *current_p++ = 0;
    }
    ret |= cnc_edid_assemble_checksum(edid, 1); // block 1
    return EDID_SUCCESS;
}

// creates the binary version of the edid from the data in the cnc_edid_t
int cnc_edid_assemble(cnc_edid_t *edid) {
    int ret;
// This if 0 version would allow the assembler to determine the number of ext blocks
//  necessary and pass that number to the main block for inclusion as the n_ext_blocks.
// As it is now there is only ever zero or one, so the 2nd version is more manageable.
#if 0
    ret = cnc_edid_assemble_extensions(edid);
    ret |= cnc_edid_assemble_main(edid);
    if(ret == EDID_SUCCESS) {
        edid->edid_bin_valid = EDID_BIN_VALID;
    }
#else
    ret = cnc_edid_assemble_main(edid);
    if(ret == EDID_ERROR) {
        printf("%s: main block assembly failed, giving up\n", __FUNCTION__);
        return ret;
    }
    if(ret) {
        ret = cnc_edid_assemble_extensions(edid);
    }
    if(ret == EDID_SUCCESS) {
        edid->edid_bin_valid = EDID_BIN_VALID;
    }
#endif
    return ret;
}
