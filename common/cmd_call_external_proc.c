#include <common.h>
#include <command.h>

#define EXTERNAL_PIC_PROCESSES_GET_KEY_WORD_CMD 'K'
#define EXTERNAL_PIC_PROCESSES_GET_FILE_SIZE_CMD 'F'
#define EXTERNAL_PIC_PROCESSES_GET_CONTEXT_SIZE_CMD 'C'

#define debug(fmt,args...)	printf (fmt ,##args)

#define EXTERNAL_PROCS_MAX_ARGC 8
DECLARE_GLOBAL_DATA_PTR;

struct external_code_attr{
  unsigned int key_word;
  unsigned char *addr;
  unsigned int file_size;
  unsigned int context_size;  
};

int do_call_external_proc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#define ALIGNED_PIC_PROC_ENTRY CONFIG_ALIGNED_PIC_PROC_MEM_BASE  
#define PIC_PROC_HAS_SEPERATE_SPACE
  extern unsigned long do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);
  static unsigned int magic[] = {0xe1a00000, 0xe1a00000};
  unsigned int *app;
  int ret=0;
  struct external_code_attr this_code_attr[16]={0};
  int mode;
  unsigned char *ori_buf=NULL;
  unsigned char *aligned_buf=NULL;
  int i;
  int found=0;
  char *p=NULL;

  if(argc > EXTERNAL_PROCS_MAX_ARGC)
  {
    printf("The count of arguments should be less than %d.\n",EXTERNAL_PROCS_MAX_ARGC);
    return -1;
  }

  if(1 == argc)
  {
    printf("%s",cmdtp->help);
    return -1;
  }
#ifndef PIC_PROC_HAS_SEPERATE_SPACE
  ori_buf=(unsigned char *)malloc(CONFIG_UBOOT_EXTERNAL_PROC_LEN);
  if(NULL==ori_buf)
  {
    printf("%s:malloc failed,size is 0x%X bytes.\n",__FUNCTION__,CONFIG_UBOOT_EXTERNAL_PROC_LEN);
    return -1;    
  }
  aligned_buf=(unsigned char *)(((int)(ori_buf+(sizeof(int)-1)))&(~(sizeof(int)-1)));
  debug("ori_buf=[0x%X],aligned_buf=[0x%X].\n",ori_buf,aligned_buf);
#else  
  aligned_buf=ALIGNED_PIC_PROC_ENTRY;
#endif
  
  memcpy(aligned_buf,(gd->relocaddr+_bss_end_ofs),CONFIG_UBOOT_EXTERNAL_PROC_LEN);
  app = aligned_buf;

  for(i=0;i<sizeof(this_code_attr)/sizeof(struct external_code_attr);i++)
  {
    char *argument[1];
    char argv_0[2]={0};
    unsigned int crc32_cal_starts;
    unsigned int crc32_cal_hdmi;
    argument[0]=argv_0;
#if 0    
    crc32_cal_starts=crc32 (0, (uchar *)app, 0x184);
    crc32_cal_hdmi=crc32 (0, (uchar *)app, 0x6164);
    debug("\n\n\ncrc32_cal_starts=[0x%X],crc32_cal_hdmi=[0x%X].\n\n\n",crc32_cal_starts,crc32_cal_hdmi);
#endif
    
    if(app[0] == magic[0] && app[1] == magic[1]) 
    {
      argv_0[0]=EXTERNAL_PIC_PROCESSES_GET_KEY_WORD_CMD;
      ret = do_go_exec((void*)app, 1, argument);
      if(ret>0)
      {
        this_code_attr[i].key_word=ret;
      }
      argv_0[0]=EXTERNAL_PIC_PROCESSES_GET_FILE_SIZE_CMD;
      ret = do_go_exec((void*)app, 1, argument);
      if(ret>0)
      {
        this_code_attr[i].file_size=ret;
      }
      argv_0[0]=EXTERNAL_PIC_PROCESSES_GET_CONTEXT_SIZE_CMD;
      ret = do_go_exec((void*)app, 1, argument);
      if(ret>0)
      {
        this_code_attr[i].context_size=ret;
      }
      this_code_attr[i].addr=app;
      app=((unsigned char*)app+this_code_attr[i].file_size); //try to get the next PIC processes.
    }
    else
    {
      break;
    }
  }
  
  if(app[0] == magic[0] && app[1] == magic[1])
  {
    printf("Too many PIC processes! We only contains %d proecesses.\n",sizeof(this_code_attr)/sizeof(struct external_code_attr));
  }

  //skip the "call_external_proc" command.
  argc-=1;
  argv++;  

  if(0==strcmp(argv[0],"list"))
  {
    for(i=0;i<sizeof(this_code_attr)/sizeof(struct external_code_attr);i++)
    {
      if(0!=this_code_attr[i].key_word)
      {
        p=(char *)(&this_code_attr[i].key_word);
        printf("%d:key_word=[%c%c%c%c],addr=[0x%X],file_size=[0x%X],context_size=[0x%X].\n",
          i,p[3],p[2],p[1],p[0],this_code_attr[i].addr,this_code_attr[i].file_size,this_code_attr[i].context_size);
      }
      else
      {
        break;
      }
    }
  }
  else
  {
    int key_num=(argv[0][0]<<24|argv[0][1]<<16|argv[0][2]<<8|argv[0][3]);
    for(i=0;i<sizeof(this_code_attr)/sizeof(struct external_code_attr);i++)
    {
      if(key_num == this_code_attr[i].key_word)
      {
        unsigned char *app_ori_buf=NULL;
        unsigned char *app_aligned_buf=NULL;
        
#ifndef PIC_PROC_HAS_SEPERATE_SPACE        
        app_ori_buf=(unsigned char *)malloc(this_code_attr[i].file_size);
        if(NULL==app_ori_buf)
        {
          printf("%s:malloc failed,size is 0x%X bytes.\n",__FUNCTION__,this_code_attr[i].context_size);
          break;    
        }
        app_aligned_buf=(unsigned char *)(((int)(app_ori_buf+(sizeof(int)-1)))&(~(sizeof(int)-1)));
        debug("app_ori_buf=[0x%X],app_aligned_buf=[0x%X].\n",app_ori_buf,app_aligned_buf);
#else        
        app_aligned_buf=(unsigned int*)ALIGNED_PIC_PROC_ENTRY;
#endif
        memcpy(app_aligned_buf,this_code_attr[i].addr,this_code_attr[i].context_size);
        ret = do_go_exec((void*)app_aligned_buf, argc, argv);
#ifndef PIC_PROC_HAS_SEPERATE_SPACE        
        if(NULL != app_ori_buf)
        {
          free(app_ori_buf);
          app_ori_buf=NULL;
        }        
#endif    
        found=1;
      }
    }  
    
    if(0 == found)
    {
      printf("Can not find any PIC processes whose key word is [%s].\n",argv[0]);
    }
  }
  
#ifndef PIC_PROC_HAS_SEPERATE_SPACE
  if(NULL != ori_buf)
  {
    free(ori_buf);
    ori_buf=NULL;
  }
#endif

  return -1;

}

U_BOOT_CMD(
		call_external_proc, EXTERNAL_PROCS_MAX_ARGC, 1, do_call_external_proc,
		"call external PIC process.\n",
		"list - list attribute of all the PIC processes\n"
		"call_external_proc key_word [args..] - call the process whose key word is \"key_word\".\n"		
	  );

