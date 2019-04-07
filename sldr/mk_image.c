
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "config.h"
#include "ddr_config.h"

#define  HASH_AREA_ADDR 	0x2c00
#define SCS_PARAM_AREA_SIZE 252
unsigned int MarketSegmentID = 0x0;
struct _ExternalParamCfg
{
	unsigned int  SCSAuxCodeSize;
	unsigned int  SCSHashAreaAddr;
	unsigned int  SCSHashAreaSize;
	unsigned int  BootLoaderDRAMAddr;    
	unsigned int  DDRCfgParameter[8];
	unsigned int  NandBlockSize;
	unsigned char BadBlockDetectEnable;
	unsigned char Data[SCS_PARAM_AREA_SIZE - 53];
}ExternalParamCfg;


long GetFileSize(char * FileName)
{
	struct stat buf;
	if (stat(FileName, &buf) == 0){
		printf("u-boot size=0x%x\n", buf.st_size);
		return (long)(buf.st_size);
	}
	else {
		printf("Cannot Get %s file size!\n", FileName);
		return 0;
	} 


}


int GenParamterBin(char *Part2File)
{
	FILE *fp;

	fp = fopen(Part2File, "wb");
	if (fp == NULL)
	{
		printf("open %s file failed\n", Part2File);
		return -1;
	}

	memset(&ExternalParamCfg, 0, sizeof(ExternalParamCfg));
	ExternalParamCfg.SCSAuxCodeSize = 8192;
	ExternalParamCfg.SCSHashAreaAddr = HASH_AREA_ADDR;
	ExternalParamCfg.SCSHashAreaSize = GetFileSize("u-boot.bin");
	ExternalParamCfg.BootLoaderDRAMAddr = 0x2000000;
	ExternalParamCfg.DDRCfgParameter[0] = DDR_PLL;
	ExternalParamCfg.DDRCfgParameter[1] = DDR_TIMING;
	ExternalParamCfg.DDRCfgParameter[2] = DDR_MOD;		
	ExternalParamCfg.DDRCfgParameter[3] = DDR_DELAY_0;
	ExternalParamCfg.DDRCfgParameter[4] = DDR_DELAY_1;
	ExternalParamCfg.DDRCfgParameter[5] = DDR_MISC;
	ExternalParamCfg.DDRCfgParameter[6] = CLK_IMPEDANCE;
	ExternalParamCfg.DDRCfgParameter[7] = CMD_IMPEDANCE;

	ExternalParamCfg.NandBlockSize = 0x20000;
	ExternalParamCfg.BadBlockDetectEnable = 0x1;

	fwrite(&ExternalParamCfg, 1, sizeof(ExternalParamCfg), fp);

	fclose(fp);

	return 0;
}

int main(int argc, char * argv[])
{
	char command[256];

	/* generate part1 */
	sprintf(command, "dd bs=516 count=1 if=/dev/zero of=sldr/part1.bin");
	system(command);

	/* generate part2 */
	if (GenParamterBin("sldr/part2_temp.bin") < 0)
	{
		printf("Generate part 2 failed\n");
		return -1;
	}

	sprintf(command, "dd bs=256 count=1 if=/dev/zero of=sldr/part2_sign.bin");
	system(command);

	system("cat sldr/part2_temp.bin sldr/part2_sign.bin > sldr/part2.bin");	

	/* generate whole image */
	system("cat sldr/part1.bin sldr/part2.bin sldr/signed_simple_loader.rom u-boot.bin > su-boot.bin");	

	system("rm sldr/part1.bin sldr/part2.bin ");
	system("rm sldr/part2_temp.bin sldr/part2_sign.bin");

	printf("create flash image done\n");

	return 0;	
}
