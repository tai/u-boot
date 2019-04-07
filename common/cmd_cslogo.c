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
#include "../drivers/cnc1800l/df/df.h"

unsigned int GFXLINEPITCHSIZE = 0;
unsigned int GFX2BASEADDRESS = 0; //not use so far
unsigned int screen_w,screen_h,picture_color_mode,pixel_width;

static int DrawPicture(int mode, unsigned char *pic_buf, int pic_w, int pic_h, int dst_x, int dst_y);
extern int gfx_init(TVOUT_MODE mode, int pic_w, int pic_h, int dst_x, int dst_y);

static char *original_argv1;

int do_cslogo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong addr, w, h, x, y;
	int mode = 0;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	original_argv1 = argv[1];
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

	if (argc != 8)  {
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
	else
		return DrawPicture(mode, (unsigned char*)addr, (int)w, (int)h, (int)x, (int)y);
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
	"logo start: Example\n"
	"    - logo output_mode color_mode start 720 576 0 0 "
	);

int go_hdmi_bin (int mode)
{
    extern unsigned long do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);
    static unsigned magic[] = {0xe92d01f0, 0xe92d0003};
    char *argv[3];
    unsigned *app;

    app = (unsigned *)(_TEXT_BASE+_end_ofs);
    /* app is now pointing to dvu.bin */

    if (app[0] == magic[0] && app[1] == magic[1]) {

        app = (unsigned *)(_TEXT_BASE+_end_ofs+app[36]);
        /* app is now pointing to hdmi.bin */

        if (app[0] == magic[0] && app[1] == magic[1]) {
            argv[0] = "hdmi.bin";
            argv[1] = original_argv1;
            argv[2] = 0;
            return do_go_exec((void*)app, 2, argv);
        }
        /* I didn't find the magic words, so that means the app is not there */
        printf("hdmi.bin app is missing\n");
        return -2;
    }

    /* I didn't find the magic words, so that means the app is not there */
    printf("dvu.bin app is missing\n");
    return -1;

}

static int DrawPicture(int mode,unsigned char *pic_buf, int pic_w, int pic_h, int dst_x, int dst_y)
{
	int i;
	unsigned char *psrcbuf;
	unsigned char *pdstbuf;

	//printf("GFXLINEPITCHSIZE = %d, screen_h = %d\n",GFXLINEPITCHSIZE,screen_h);
	/* clear screen */
	memset((void*)GFXBASEADDRESS, 0, GFXLINEPITCHSIZE * screen_h);

	//printf("GFXBASE=0x%x, GFXLINEPITCHSIZE=0x%x, Pic_buf=0x%x, DrawPicture:w=%d, h=%d, x=%d, y=%d\n",
    //    (unsigned int)GFXBASEADDRESS,(unsigned int) GFXLINEPITCHSIZE,(unsigned int) pic_buf, pic_w, pic_h, dst_x, dst_y);
 	//printf("pixel_width = %d\n",pixel_width);

	gfx_init(mode, pic_w, pic_h, dst_x, dst_y);

	psrcbuf = (unsigned char*)pic_buf;
	pdstbuf = (unsigned char*)(GFXBASEADDRESS + (dst_y * GFXLINEPITCHSIZE) + dst_x * pixel_width);
	for(i = 0; i < pic_h; i++)
	{
		memcpy(pdstbuf, psrcbuf, pic_w * pixel_width);
		psrcbuf += (pic_w*pixel_width);
		pdstbuf += GFXLINEPITCHSIZE;
	}

//	write_hd2sd_cmd(mode);

	/* call the standalone U-Boot app that does most of the work */
	return go_hdmi_bin(mode);
}
