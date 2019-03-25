#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define debug(...)

int change_rgb888_to_rgb565(char *FileIn,char* FileOut,const int width, const int height)
{
	unsigned char Bytes_1[4];
	unsigned char Bytes_2[4]={0,0,0,0};
	unsigned short pixel;
	unsigned int i =0;
	unsigned int j=0;
	FILE *fpFileIn =NULL;
	FILE *fpFileOut =NULL;
	FILE *fpTmp =NULL;
	fpFileIn =fopen(FileIn,"rb");
	fpTmp =fopen("tmpfile","w+b");
	unsigned short (*pix_array)[height][width]=malloc(width*height*2);
	
	fseek(fpFileIn, 0x36L, SEEK_SET);

	if(width*height>0)
	{
		char tmp;
		for(i=0;i<width*height;i++)
		{
			int n;
			if(1 !=(n=fread(Bytes_1,3,1,fpFileIn)))
			{
				debug("%s,%d,i=[%d],n=[%d].\n",__FILE__,__LINE__,i,n);
				//perror("fread:");
			}
			else
			{
				pixel=(((Bytes_1[0]>>3)<<0) | ((Bytes_1[1]>>2)<<5) | ((Bytes_1[2])>>3)<<11);
				fwrite(&pixel,2,1,fpTmp);
			}
		}
	}

	fclose(fpFileIn);
	fclose(fpTmp);

	fpFileOut=fopen(FileOut,"w+b");
	fpTmp=fopen("tmpfile","rb");
	
	for(i=0;i<height;i++)
	{
		fread((*pix_array)[i],width*2,1,fpTmp);
	}

	for(i=0;i<height;i++)
	{
		fwrite((*pix_array)[height-1-i],width*2,1,fpFileOut);
	}		

	fclose(fpTmp);
	fclose(fpFileOut);	
	remove("tmpfile");
	free(pix_array);

	return 1;
}

int change_rgb888_to_argb8888(char *FileIn,char* FileOut,const int width, const int height)
{
	unsigned char Bytes_1[4];
	unsigned char Bytes_2[4]={0,0,0,0};
	unsigned int pixel;
	unsigned int i =0;
	unsigned int j=0;
	FILE *fpFileIn =NULL;
	FILE *fpFileOut =NULL;
	FILE *fpTmp =NULL;
	
	fpFileIn =fopen(FileIn,"rb");
	if(!fpFileIn)
		perror("fopen:");
	fpTmp =fopen("tmpfile","w+b");
	if(!fpTmp)
		perror("fopen:");
	
	unsigned int (*pix_array)[height][width]=malloc(width*height*4);
		
	fseek(fpFileIn, 0x36L, SEEK_SET);
	
	if(width*height>0)
	{
		char tmp;
		for(i=0;i<width*height;i++)
		{
			int n;
			if(1 !=(n=fread(Bytes_1,3,1,fpFileIn)))
			{
				debug("%s,%d,i=[%d],n=[%d].\n",__FILE__,__LINE__,i,n);
				//perror("fread:");
			}
			else
			{
				//pixel=(Bytes_1[0]<<0) | (Bytes_1[1]<<8) | (Bytes_1[2]<<16) | (0xa0<<24);
				Bytes_1[3]=0xff;
				fwrite(Bytes_1,4,1,fpTmp);
			}
		}
	}

	fclose(fpFileIn);
	fclose(fpTmp);
	
	fpFileOut=fopen(FileOut, "w+b");
	if(!fpFileOut)
		perror("fopen:");
	fpTmp =fopen("tmpfile", "rb");
	if(!fpTmp)
		perror("fopen:");
			
	for(i=0;i<height;i++)
	{
		fread((*pix_array)[i],width*4,1,fpTmp);
	}

	for(i=0;i<height;i++)
	{
		fwrite((*pix_array)[height-1-i],width*4,1,fpFileOut);
	}		

	fclose(fpFileOut);	
	fclose(fpTmp);
	remove("tmpfile");
	free(pix_array);	

	return 1;
}

void print_usage(char *cmd)
{
	printf("This program can convert a 24 bits bmp(rgb888 format, not compressed) file to\n");
	printf("  a binary file which can be used by \"logo\" command in Cavium uboot.\n");
	printf("Note that:\n");
	printf("  the source file must be a rgb888 and NOT compressed bmp file.\n");
	printf("Usage:\n");
	printf("  %s [{565,8888}] <input_file> <output_file> <width> <height> \n", cmd);
}

int main(int argc,char *argv[])
{
	int width, height;
	int format;
	struct stat sb;
	int bmp_file;
	
	if(argc < 5){
		print_usage(argv[0]);
		return -1;
	}
	
	format = atoi(argv[1]);
	if(565 != format && 8888 != format)
	{
		printf("You must specify the format of the bmp file in the second argument.\n");
		print_usage(argv[0]);
		return -1;		
	}
	
	width = atoi(argv[4]);
	height = atoi(argv[5]);

	if(-1 != (bmp_file = open(argv[2],O_RDONLY)))
	{
		if(fstat(bmp_file,&sb))
          printf("Error: Can not open the source bmp file [%s].\n",argv[2]);
		close(bmp_file);
		if(sb.st_size != (0x36+width*height*3))
		{
			printf("BMP file size error!\n");	
			print_usage(argv[0]);
			return -1;				
		}
	}	
		
	printf("Converting bmp logo %s to %s (%d x %d), the format is %d.\n", argv[2], argv[3], width, height, format);

	if(565 == format)
	{
		change_rgb888_to_rgb565(argv[2], argv[3], width, height);
	}
	else if(8888 == format)
	{
		change_rgb888_to_argb8888(argv[2], argv[3], width, height);
	}
	
	return 0;
}



