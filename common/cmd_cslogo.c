/****************************************************************************
  * Copyright (C) 2011 Cavium Inc.
  * All rights reserved
  *
  * [RELEASE HISTORY]                           
  * VERSION  DATE       AUTHOR                  DESCRIPTION
  * 0.1      11-10-11       Jia Ma           		Original
  ****************************************************************************
*/

#include <configs/cnc1800l.h>
#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <malloc.h>
#include "../drivers/cnc1800l/cncedid/cnc_edid.h"
#include "../drivers/cnc1800l/df/df.h"
unsigned int GFXLINEPITCHSIZE = 0;
unsigned int GFX2BASEADDRESS = 0; //not use so far
unsigned int screen_w,screen_h,picture_color_mode,pixel_width;

static int DrawPicture(int mode, unsigned char *pic_buf, int pic_w, int pic_h, int dst_x, int dst_y);

cnc_edid_t edid_storage;
int tvoutmode_supported_res[TVOUT_MODE_MAX] = {0};
int audmode_supported[CSAUD_HDMI_MAX] = {0};

int do_edid(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int* ret_val = 0xf000000;//hardcode majia.
	cnc_edid_t *edid = &edid_storage;
	int res = -1;
	char* bootargs = NULL;
	char* temp_char = NULL;
	char *hdmi_argv[4]={0};
	char mode_str[5]={0};

	sprintf(mode_str,"%d",255);
	hdmi_argv[0]="call_external_proc";
	hdmi_argv[1]="HDMI";
	hdmi_argv[2]="255";
	do_call_external_proc(NULL, 0, 3, hdmi_argv);

	if(0xffffffff == *((int*)0xf000000)){
		printf("HDMI not plug in or Read EDID info failed!\n");
		return;
	}
	cnc_edid_init(edid);
	memcpy(edid->edid_bin, 0xf000004, ret_val[0]);
	edid->edid_bin_valid = EDID_BIN_VALID;
	edid->edid_bin_length = ret_val[0];

	cnc_edid_parse(edid);
	temp_char = getenv("PRINT_EDID");
	if(NULL != temp_char){
		printf("Original EDID:\n");
		cnc_edid_print(edid);
	}
	res=cnc_edid_filter(edid, tvoutmode_supported_res, audmode_supported);
	if(NULL != temp_char){
		printf("Filtered EDID:\n");
		cnc_edid_print(edid);
	}
	temp_char = NULL;
	cnc_edid_clear_bin(edid);
	//        cnc_edid_assemble(edid);
	printf("best res = %d\n",res);

//set env
	switch(res){
		case 0:
		case 2:
		case 8:
		case 10:
		case 11:
			//720x576, uboot HDMI logo not support it
			res = -1;
			break;
		case 1:
		case 3:
		case 9:
			//720x480, uboot HDMI logo not support it
			res = -1;
			break;
		case 4:
		case 5:
		case 20:
			//1280x720
			temp_char = getenv("LOGO_ADDR_720P");
			setenv("LOGO_ADDR", temp_char);
			temp_char = getenv("LOGO_SIZE_720P");
			setenv("LOGO_SIZE", temp_char);
			temp_char = getenv("LOGO_X_720P");
			setenv("LOGO_X", temp_char);
			temp_char = getenv("LOGO_Y_720P");
			setenv("LOGO_Y", temp_char);
			temp_char = getenv("LOGO_WIDTH_720P");
			setenv("LOGO_WIDTH", temp_char);
			temp_char = getenv("LOGO_HEIGHT_720P");
			setenv("LOGO_HEIGHT", temp_char);
			break;
		case 6:
		case 7:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			//1920x1080
			temp_char = getenv("LOGO_ADDR_1080P");
			setenv("LOGO_ADDR", temp_char);
			temp_char = getenv("LOGO_SIZE_1080P");
			setenv("LOGO_SIZE", temp_char);
			temp_char = getenv("LOGO_X_1080P");
			setenv("LOGO_X", temp_char);
			temp_char = getenv("LOGO_Y_1080P");
			setenv("LOGO_Y", temp_char);
			temp_char = getenv("LOGO_WIDTH_1080P");
			setenv("LOGO_WIDTH", temp_char);
			temp_char = getenv("LOGO_HEIGHT_1080P");
			setenv("LOGO_HEIGHT", temp_char);
			break;
		case 17:
			//640x480 uboot HDMI logo not support it
			res = -1;
			break;
		case 18:
			//800x600 uboot HDMI logo not support it
			res = -1;
			break;
		case 19:
			//1024x768 uboot HDMI logo not support it
			res = -1;
			break;
		default:
			//720x576 uboot HDMI logo not support it
			res = -1;
			break;
	}

	if(-1 == res){
		printf("Don't support format!\n");
		return;
	}
	else{
		temp_char = NULL;
	// set bootargs outputmode parameter
		bootargs = getenv("bootargs");
//		printf("bootargs = %s\n", bootargs);
		temp_char = strstr(bootargs,"outputmode=");
//		printf("temp_char = %s\n",temp_char);
		if(NULL == temp_char){
			temp_char = malloc(strlen(bootargs) + 20);
			if(0 == temp_char){
				printf("Not enough memory!\n");
				return;
			}
			memcpy(temp_char, bootargs, strlen(bootargs));
			sprintf(temp_char+strlen(bootargs)," outputmode=%d",res);
			setenv("bootargs", temp_char);
		}
		else{
			int num1 = 0;
			int num2 = 0;
			char* aaa = strchr(temp_char, '=');
			char* bbb = strchr(temp_char, ' ');

			num1 = aaa - bootargs;
//	printf("num1 = %d\n",num1);
//			printf("%s\n",aaa+1);
			if(NULL == bbb){
				temp_char = malloc(strlen(bootargs) + 20);
				if(0 == temp_char){
					printf("Not enough memory!\n");
					return;
				}
				memcpy(temp_char, bootargs, num1+1);
				sprintf(temp_char+num1+1,"%d",res);
			}
			else{
				num2 = bbb - bootargs;
//	printf("num2 = %d\n",num2);
				temp_char = malloc(strlen(bootargs) + 20);
				if(0 == temp_char){
					printf("Not enough memory!\n");
					return;
				}
				strncpy(temp_char, bootargs, num1+1);
				sprintf(temp_char+num1+1,"%d",res);
//				printf("strlen(temp_char) = %d\n",strlen(temp_char));
				strncat(temp_char+strlen(temp_char), bbb,strlen(bbb));
			}
		}
		setenv("bootargs", temp_char);
		sprintf(temp_char, "%d", res);
		setenv("OUTPUTMODE",temp_char);
	}
}

int do_cslogo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong addr, w, h, x, y;
	int mode = 0;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	mode = simple_strtoul(argv[1], NULL, 0);
	picture_color_mode = simple_strtoul(argv[2], NULL, 0);

	printf("mode = %d, picture_color_mode = %d\n",mode,picture_color_mode);

	switch(mode){
		case 0:
		case 2:
		case 8:
		case 10:
		case 11:
			screen_w = 720;
			screen_h = 576;
			GFXLINEPITCHSIZE = 720*2;
			break;
		case 1:
		case 3:
		case 9:
			screen_w = 720;
			screen_h = 480;
			GFXLINEPITCHSIZE = 720*2;
			break;
		case 4:
		case 5:
		case 20:
			screen_w = 1280;
			screen_h = 720;
			GFXLINEPITCHSIZE = 1280*2;
			break;
		case 6:
		case 7:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			screen_w = 1920;
			screen_h = 1080;
			GFXLINEPITCHSIZE = 1920*2;
			break;
		case 17:
			screen_w = 640;
			screen_h = 480;
			GFXLINEPITCHSIZE = 640*2;
			break;
		case 18:
			screen_w = 800;
			screen_h = 600;
			GFXLINEPITCHSIZE = 800*2;
			break;
		case 19:
			screen_w = 1024;
			screen_h = 768;
			GFXLINEPITCHSIZE = 1024*2;
			break;
		default:
			screen_w = 720;
			screen_h = 576;
			GFXLINEPITCHSIZE = 720*2;
			break;
	}

	pixel_width = 2;
	
	if(picture_color_mode == 6){
		GFXLINEPITCHSIZE = GFXLINEPITCHSIZE*2;
		pixel_width = 4;
	}
	printf("1GFXLINEPITCHSIZE = %d, screen_h = %d\n",GFXLINEPITCHSIZE,screen_h);
	addr = simple_strtoul(argv[3], NULL, 16);
	if (argc == 4) 
		return DrawPicture(mode, (unsigned char*)addr, 720, 576, 0, 0);

	if (argc < 8)  {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	w = simple_strtoul(argv[4], NULL, 0);
	h = simple_strtoul(argv[5], NULL, 0);
	x = simple_strtoul(argv[6], NULL, 0);
	y = simple_strtoul(argv[7], NULL, 0);
	printf("2GFXLINEPITCHSIZE = %d, screen_h = %d\n",GFXLINEPITCHSIZE,screen_h);
	if(w > screen_w || h > screen_h || x > screen_w || y > screen_h || w + x > screen_w || h + y > screen_h){
		printf("picture is too large!\n");
		return 1;
	}
	else{
		DrawPicture(mode, (unsigned char*)addr, (int)w, (int)h, (int)x, (int)y);
	}

	if (argc < 10){
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	else{
		mode = simple_strtoul(argv[8], NULL, 0);
		addr = simple_strtoul(argv[9], NULL, 0);
		printf("mode = %d, addr = 0x%x\n",mode,addr);
		sdoutput(mode, addr);
	}

	return 0;
}

U_BOOT_CMD(
	logo,	10,	1,	do_cslogo,
	"logo    -display logo picture on TV Screen",
	"    - output mode(TVOUT_MODE): refer to the struct CSTVOUT_MODE in cstvout.h\n"
	"    - color mode(DF_GFX_COLOR_FORMAT): refer to th struct CSOSD_COLOR_FORMAT in csosd.h\n"
	"    - start(hex): 	memory start address\n"
	"    - width(decimal): 	picture width\n"
	"    - height(decimal): picture height\n"
	"    - dstX(decimal): 	the top-left X coordinate of screen\n"
	"    - dstY(decimal): 	the top-left Y coordinate of screen\n"
	"    - otuput_mode_cvbs(TVOUT_MODE): refer to the struct CSTVOUT_MODE in cstvout.h\n"
	"    - yuv_start(hex): 	memory start address of yuv data\n"
	"logo start: Example-No CVBS output\n"
	"    - logo output_mode color_mode start 720 576 0 0 "
	"    Example-Has CVBS output"
	"    - logo output_mode color_mode start 720 576 0 0 output_mode_cvbs yuv_start"
	);

U_BOOT_CMD(
	edid,	1,	1,	do_edid,
	"logo    -display logo picture on TV Screen",
	"    - output mode(TVOUT_MODE): refer to the struct CSTVOUT_MODE in cstvout.h\n"
	"    - color mode(DF_GFX_COLOR_FORMAT): refer to th struct CSOSD_COLOR_FORMAT in csosd.h\n"
	"    - start(hex): 	memory start address\n"
	"    - width(decimal): 	picture width\n"
	"    - height(decimal): picture height\n"
	"    - dstX(decimal): 	the top-left X coordinate of screen\n"
	"    - dstY(decimal): 	the top-left Y coordinate of screen\n"
	"    - otuput_mode_cvbs(TVOUT_MODE): refer to the struct CSTVOUT_MODE in cstvout.h\n"
	"    - yuv_start(hex): 	memory start address of yuv data\n"
	"logo start: Example-No CVBS output\n"
	"    - logo output_mode color_mode start 720 576 0 0 "
	"    Example-Has CVBS output"
	"    - logo output_mode color_mode start 720 576 0 0 output_mode_cvbs yuv_start"
	);

static int DrawPicture(int mode,unsigned char *pic_buf, int pic_w, int pic_h, int dst_x, int dst_y)
{
	int i;
	unsigned char *psrcbuf;
	unsigned char *pdstbuf;

	printf("GFXLINEPITCHSIZE = %d, screen_h = %d\n",GFXLINEPITCHSIZE,screen_h);
	/* clear screen */
	memset((void*)GFXBASEADDRESS, 0, GFXLINEPITCHSIZE * screen_h);

	printf("GFXBASE=0x%x, GFXLINEPITCHSIZE=0x%x, Pic_buf=0x%x, DrawPicture:w=%d, h=%d, x=%d, y=%d\n",GFXBASEADDRESS, GFXLINEPITCHSIZE, pic_buf, pic_w, pic_h, dst_x, dst_y);
 	printf("pixel_width = %d\n",pixel_width);

	gfx_init(mode, pic_w, pic_h, dst_x, dst_y);

	psrcbuf = (unsigned char*)pic_buf;
	pdstbuf = (unsigned char*)(GFXBASEADDRESS + (dst_y * GFXLINEPITCHSIZE) + dst_x * pixel_width);

	for(i = 0; i < pic_h; i++){
		memcpy(pdstbuf, psrcbuf, pic_w * pixel_width);
		psrcbuf += (pic_w*pixel_width);
		pdstbuf += GFXLINEPITCHSIZE;
	}

//	write_hd2sd_cmd(mode);

	return 0;
}

