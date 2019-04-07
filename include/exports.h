#ifndef __EXPORTS_H__
#define __EXPORTS_H__

#ifndef __ASSEMBLY__

#include <common.h>
typedef enum _CLOCK_RESET_
{
  _do_reset =0,
  _do_set   =1,
}CLOCK_RESET;

/* These are declarations of exported functions available in C code */
unsigned long get_version(void);
int  getc(void);
int  tstc(void);
void putc(const char);
void puts(const char*);
int printf(const char* fmt, ...);
void install_hdlr(int, interrupt_handler_t*, void*);
void free_hdlr(int);
void *malloc(size_t);
void free(void*);
void __udelay(unsigned long);
unsigned long get_timer(unsigned long);
int vprintf(const char *, va_list);
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);
char *getenv (char *name);
int setenv (char *varname, char *varvalue);
long simple_strtol(const char *cp,char **endp,unsigned int base);
int strcmp(const char * cs,const char * ct);
int ustrtoul(const char *cp, char **endp, unsigned int base);
#if defined(CONFIG_CMD_I2C)
int i2c_write (uchar, uint, int , uchar* , int);
int i2c_read (uchar, uint, int , uchar* , int);
#endif
int strncmp(const char * cs,const char * ct,size_t count);
size_t strlen(const char * s);
int SEC_Reset(void);
int SEC_init(void);
int CHL_Init(unsigned int chl);
int SetCryptoMode(unsigned int chl, int emi, unsigned char *Ivector, int size);
int SetHostKey(unsigned int chl, int emi, unsigned char * hostkey, int size);
int CHL_Check_InBuffer_FreeSize(unsigned int chl);
int Block_CHL_Write_Data(unsigned int chl,  char * buf, unsigned int len);
int CHL_Start(unsigned int chl);
int GetCryptoDone(unsigned int chl, unsigned char* status);
int Block_CHL_Read_Data(unsigned int chl, char* buf, unsigned int len);
int CHL_Stop(unsigned int chl);
int Stream_CHL_Write_Data(unsigned int chl, char * buf, unsigned int len);
int CHL_Check_OutBuffer_DataSize(unsigned int chl);
int Stream_CHL_Read_Data(unsigned int chl, char * buf, unsigned int len);
int CHL_Flush(unsigned int chl);
#include <spi.h>

void app_startup(char * const *);
void clock_hdmi_reset(CLOCK_RESET ResetOrSet);

#endif    /* ifndef __ASSEMBLY__ */

enum {
#define EXPORT_FUNC(x) XF_ ## x ,
#include <_exports.h>
#undef EXPORT_FUNC

	XF_MAX
};

#define XF_VERSION	6

#if defined(CONFIG_X86)
extern gd_t *global_data;
#endif

#endif	/* __EXPORTS_H__ */
