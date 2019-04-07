/**
 * @file   crypto_drv.h
 * @author Fan Xiaodong <xiaodong.fan@caviumnetworks.com>
 * @date   Dec. 2011
 * 
 * @brief  
 * 
 * 
 */


#ifndef __CRYPTO_H__
#define __CRYPTO_H__




#define CNC_SEC_MB_BASE       0xfffff800

#define MAILBOX0              (CNC_SEC_MB_BASE+0x00)
#define MAILBOX1              (CNC_SEC_MB_BASE+0x04)
#define MAILBOX2              (CNC_SEC_MB_BASE+0x08)
#define MAILBOX3              (CNC_SEC_MB_BASE+0x0c)
#define MAILBOX4              (CNC_SEC_MB_BASE+0x10)
#define MAILBOX5              (CNC_SEC_MB_BASE+0x14)
#define MAILBOX6              (CNC_SEC_MB_BASE+0x18)
#define MAILBOX7              (CNC_SEC_MB_BASE+0x1c)

#define CNC_CRYPTO_BASE       0x80240000
#define TDES_INT_REG_ADDR     (CNC_CRYPTO_BASE+0x00)
#define TDES_INT_ENB_ADDR     (CNC_CRYPTO_BASE+0x04)
#define TDES_INT_CLS_ADDR     (CNC_CRYPTO_BASE+0x08)
#define TDES_INT_RAW_REG_ADDR (CNC_CRYPTO_BASE+0x0c)

//#define SEC_MAX_CHL_NUM  3
#define CHL_BASE(chl)         (CNC_CRYPTO_BASE + chl * 0x100)

#define TDES_IV0(chl)         (CHL_BASE(chl)+0x10)
#define TDES_IV1(chl)         (CHL_BASE(chl)+0x14)
#define TDES_IV2(chl)         (CHL_BASE(chl)+0x18)
#define TDES_IV3(chl)         (CHL_BASE(chl)+0x1c)


#define TDES_DATA_SIZE(chl)   (CHL_BASE(chl)+0x20)

#define TDES_CONT(chl)        (chl?(CHL_BASE(chl)+0x20):(CHL_BASE(chl)+0x24))
#define TDES_COM(chl)         (CHL_BASE(chl)+0x28)

#define TDES_KEY0(chl)        (CHL_BASE(chl)+0x30)
#define TDES_KEY1(chl)        (CHL_BASE(chl)+0x34)
#define TDES_KEY2(chl)        (CHL_BASE(chl)+0x38)
#define TDES_KEY3(chl)        (CHL_BASE(chl)+0x3c)
#define TDES_KEY4(chl)        (CHL_BASE(chl)+0x40)
#define TDES_KEY5(chl)        (CHL_BASE(chl)+0x44)
#define TDES_KEY6(chl)        (CHL_BASE(chl)+0x48)
#define TDES_KEY7(chl)        (CHL_BASE(chl)+0x4c)

#define TDES_CTR_CNT_UP(chl)  (CHL_BASE(chl)+0x60)

#define SAR_TS(chl)           (CNC_CRYPTO_BASE + 0x64)
#define DAR_TS0(chl)          (CNC_CRYPTO_BASE + 0x68)
#define BLOCK_FIFO            (CNC_CRYPTO_BASE + 0x80)
#define BLOCK_FIFO_CNT        (CNC_CRYPTO_BASE + 0x84)

#define SRC_UP_ADDR_TS(chl)   (CHL_BASE(chl)+0x64)
#define SRC_LOW_ADDR_TS(chl)  (CHL_BASE(chl)+0x68)
#define SRC_WR_PT_TS(chl)     (CHL_BASE(chl)+0x6c)
#define SRC_RD_PT_TS(chl)     (CHL_BASE(chl)+0x70)

#define DES_UP_ADDR_TS(chl)   (CHL_BASE(chl)+0x74)
#define DES_LOW_ADDR_TS(chl)  (CHL_BASE(chl)+0x78)
#define DES_WR_PT_TS(chl)     (CHL_BASE(chl)+0x7c)
#define DES_RD_PT_TS(chl)     (CHL_BASE(chl)+0x80)

#define SRC_THRESHOLD_TS(chl) (CHL_BASE(chl)+0x84)
#define DES_THRESHOLD_TS(chl) (CHL_BASE(chl)+0x88)


#define scSWAP(x) \
              ((unsigned int)( \
              (((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) | \
              (((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) | \
              (((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) | \
              (((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24) ))

#define scMin(a, b)    (((a) > (b))?(b):(a))


/* Register operation */
#define scwrite32(a,v) (*(volatile unsigned int *)(a) = v)
#define scread32(a)     (*(volatile unsigned int *)(a))



#define IS_TDES(x)      (((x) & 0xff0000) == 0x0)
#define IS_AES(x)       (((x) & 0x30000) == 0x10000)
#define IS_DVB(x)       (0) /*disable DVB now */

#define IS_ECB(x)       (((x) & 0xff00) == 0x0)
#define IS_CBC(x)       (((x) & 0xff00) == 0x100)

#define IS_CLEAR(x)     (((x) & 0xff) == 0x0)
#define IS_DVS042(x)    (((x) & 0xff) == 0x1)

#define IS_ENCRYPT(x)   (((x) & 0x80000000))

#define CRYPTO_CHANNEL_MAX_NUM 3
typedef enum _SEC_ERROR_CODE
{
    SEC_CORE_SUCCESS                    =  0,
    SEC_CORE_ERROR                      = -1,
    SEC_CORE_UNSUPPORTED_OPERATION      = -2,    
    SEC_CORE_INVALID_PARAM              = -3,
}SEC_ERROR_CODE;

typedef enum _SEC_ALGORITHM_IDX{
	TDES = 0,
    AES128 = 1,
	SMS4   = 2,
    AES192 = 9,
    AES256 = 13,
}SEC_ALGORITHM_IDX;


typedef enum _SEC_CIPHER_MODE_IDX{
	ECB	= 0,
	CBC,
	CFB128,
	OFB128,
	CTS,
	CTR,
	//RESERVED
}SEC_CIPHER_MODE_IDX;

typedef enum _SEC_SHORT_BLOCK_IDX{
	CLEAR 	= 0,
	DVS042  = 1,
	//RESERVED
}SEC_SHORT_BLOCK_IDX;

typedef enum _SEC_EMI
{
	DECRYPT_AES128_IDSA	    = ((AES128 << 16) & 0xFF0000) | ((CBC << 8) & 0xFF00) | ((DVS042 << 0) & 0xFF),
	DECRYPT_AES128_ECB	    = ((AES128 << 16) & 0xFF0000) | ((ECB << 8) & 0xFF00) | ((CLEAR << 0)  & 0xFF),
	DECRYPT_AES128_CIPlus	= ((AES128 << 16) & 0xFF0000) | ((CBC << 8) & 0xFF00) | ((CLEAR << 0)  & 0xFF),
	DECRYPT_TDES_CBC_DVS042 = ((TDES << 16) & 0xFF0000)  | ((CBC << 8)  & 0xFF00) | ((DVS042 << 0) & 0xFF),
    DECRYPT_DES_ECB_CLEAR	= ((TDES << 16) & 0xFF0000)  | ((ECB << 8)  & 0xFF00) | ((CLEAR << 0)  & 0xFF),

	ENCRYPT_AES128_IDSA	    = ((1<< 31) & 0xFF000000)| ((AES128 << 16)& 0xFF0000) | ((CBC << 8)& 0xFF00) | ((DVS042 << 0)& 0xFF),
    ENCRYPT_AES128_ECB	    = ((1<< 31) & 0xFF000000)| ((AES128 << 16)& 0xFF0000) | ((ECB << 8)& 0xFF00) | ((CLEAR << 0) & 0xFF),
    ENCRYPT_AES128_CIPlus	= ((1<< 31) & 0xFF000000)| ((AES128 << 16)& 0xFF0000) | ((CBC << 8)& 0xFF00) | ((CLEAR << 0) & 0xFF),
	ENCRYPT_TDES_CBC_DVS042 = ((1<< 31) & 0xFF000000)| ((TDES << 16)  & 0xFF0000) | ((CBC << 8)& 0xFF00) | ((DVS042 << 0)& 0xFF),
    ENCRYPT_DES_ECB_CLEAR	= ((1<< 31) & 0xFF000000)| ((TDES << 16)  & 0xFF0000) | ((ECB << 8)& 0xFF00) | ((CLEAR << 0) & 0xFF),    
    NO_SET =0xFF000000,
}SEC_EMI;


typedef enum _CRYPTO_CHANNEL_MODE 
{
    BLOCK_DMA_MODE  = 0,
    STREAMING_MODE  = 1,
    BLOCK_HOST_MODE = 2,
}SEC_CHANNEL_MODE;

typedef struct _CRYPTO_CHANNEL_BUFFER_PINTER
{
    unsigned int w_pointer;
    unsigned int r_pointer;

}SEC_CHANNEL_BUFFER_POINTER;

typedef struct _CRYPTO_CHANNEL_INFO
{
    unsigned int  open_status;
    unsigned int  p_chl_mem_base;
    unsigned int  chl_mem_size;
    unsigned int  p_in_buffer_base;
    unsigned int  in_buffer_size;
    unsigned int  p_out_buffer_base;
    unsigned int  out_buffer_size;
    SEC_CHANNEL_MODE  mode; /* 0: for block; 1: for streaming */
    SEC_CHANNEL_BUFFER_POINTER input_buffer;
    SEC_CHANNEL_BUFFER_POINTER output_buffer;
    unsigned int count;
    unsigned long long write_count;
    unsigned long long read_count;
    unsigned int term_last_flag;
    SEC_EMI       emi; 
}SEC_CHANNEL_INFO;

SEC_ERROR_CODE SEC_Reset(void);
SEC_ERROR_CODE SEC_init(void);
SEC_ERROR_CODE SEC_term(void);
SEC_ERROR_CODE SetHostKey (unsigned int chl, 
                          SEC_EMI emi, 
                          unsigned char * hostkey, 
                          int size);
SEC_ERROR_CODE SetCryptoMode (unsigned int        chl,
                              SEC_EMI             emi,
                              unsigned char *     Ivector,
                              int                 size);
SEC_ERROR_CODE GetCryptoIV(unsigned int          chl,
                           SEC_EMI               emi,
                           unsigned char *       Ivector,
                           int *                 size);
SEC_ERROR_CODE CHL_Flush(unsigned int chl);
SEC_ERROR_CODE CHL_Start (unsigned int chl);
SEC_ERROR_CODE CHL_Stop(unsigned int chl);
SEC_ERROR_CODE CHL_Reset(unsigned int chl);
SEC_ERROR_CODE SetBlockCHLMode(unsigned int chl, 
                               SEC_CHANNEL_MODE mode);
SEC_ERROR_CODE GetCryptoDone (unsigned int   chl, 
                              unsigned char* status);
SEC_ERROR_CODE Update_CHL_InWr_Pointer(unsigned int chl);
SEC_ERROR_CODE Get_CHL_InRe_Pointer(unsigned int chl);
SEC_ERROR_CODE Update_CHL_OutRe_Pointer(unsigned int chl);
SEC_ERROR_CODE Get_CHL_OutWr_Pointer(unsigned int chl);
int CHL_Check_OutBuffer_DataSize(unsigned int chl);
int CHL_Check_InBuffer_FreeSize(unsigned int chl);
int Block_CHL_Read_Data (unsigned int chl, 
                             char * buf, 
                             unsigned int len);
int Stream_CHL_Read_Data(unsigned int chl, 
                             char * buf, 
                             unsigned int len);
int Stream_CHL_Write_Data(unsigned int chl, 
                          char * buf, 
                          unsigned int len);
int Block_CHL_Write_Data(unsigned int chl, 
                         char * buf, 
                         unsigned int len);
SEC_ERROR_CODE CHL_Init(unsigned int chl);
SEC_ERROR_CODE CHL_Stop(unsigned int chl);
SEC_ERROR_CODE CHL_Start(unsigned int chl);
void CHL_Dump_Regs(unsigned int chl);

#endif

