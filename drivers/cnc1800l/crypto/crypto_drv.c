/**
 * @file   crypto_drv.c
 * @author Fan Xiaodong <xiaodong.fan@caviumnetworks.com>
 * @date   Nov 2011
 * 
 * @brief  Secrity driver on U-boot
 * 
 * 
 */

#include <common.h>
#include "sec_define.h"
#include "crypto_drv.h"


extern SEC_CHANNEL_INFO channel_info[CRYPTO_CHANNEL_MAX_NUM];


//#define DEBUG_PRINTF    printf
#define DEBUG_PRINTF(fmt, args...)

#define TIMEOUT_MAX 200000
#define CHECK_WITH_TMO(condition) ({            \
            int timeout = 0;                    \
            while (condition) {                 \
                if (timeout++ > TIMEOUT_MAX)    \
                    return SEC_CORE_ERROR;      \
                else                            \
                    udelay(1);                  \
            }                                   \
        })

#define SEC_ASSERT(x)   {                                           \
        SEC_ERROR_CODE code;                                        \
        code = (x);                                                 \
        if (code != SEC_CORE_SUCCESS)                               \
            {                                                       \
                printf("--func:%s line:%d error:%d --  \n", \
                       __FUNCTION__,__LINE__,code);                 \
                return code;                                        \
            }                                                       \
    }

SEC_CHANNEL_INFO channel_info[CRYPTO_CHANNEL_MAX_NUM];

void CHL_Dump_Regs(unsigned int chl)
{	unsigned int addr;
	unsigned int val;

    printf("Interrupt: \n");
    for (addr = (unsigned int)CNC_CRYPTO_BASE; addr <=(unsigned int)CNC_CRYPTO_BASE + 0xc; addr += 4) {
        val = scread32(addr);
        printf(" crypto_read[0x%03x] = 0x%08x \n", addr, val);
    }
    if (chl == 0) {
        printf("\nCHL 0: \n");
        for (addr = (unsigned int)CNC_CRYPTO_BASE +0x10; addr <= (unsigned int)CNC_CRYPTO_BASE +0x4c; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr, val);
        }
        for (addr = (unsigned int)CNC_CRYPTO_BASE +0x60; addr <= (unsigned int)CNC_CRYPTO_BASE +0x68; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr, val);
        }
    } else if (chl == 1){
        printf("\nCHL 1: \n");
        for (addr = (unsigned int)CNC_CRYPTO_BASE +0x110; addr <= (unsigned int)CNC_CRYPTO_BASE +0x14c; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr , val);
        }
        for (addr = (unsigned int)CNC_CRYPTO_BASE +0x160; addr <= (unsigned int)CNC_CRYPTO_BASE +0x188; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr , val);
        }
    }else if (chl == 2) {
        printf("\nCHL 2: \n");
        for (addr = (unsigned int)CNC_CRYPTO_BASE +0x210; addr <= (unsigned int)CNC_CRYPTO_BASE +0x24c; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr, val);
        }
        for (addr = (unsigned int)CNC_CRYPTO_BASE + 0x260; addr <= (unsigned int)CNC_CRYPTO_BASE +0x288; addr += 4) {
            val = scread32(addr);
            printf(" crypto_read[0x%03x] = 0x%08x \n", addr, val);
        }
    }
}


SEC_ERROR_CODE SEC_Reset(void)
{
    unsigned int status;
    status =scread32(0xB2100200);
    scwrite32(0xB2100200, status & (~(1<<5)));
    scwrite32(0xB2100200, status | (1<<5));
    scwrite32(TDES_INT_CLS_ADDR, 0xFFFFFFFF);
    //    scwrite32(TDES_INT_ENB_ADDR, 0xffffffff);
    scwrite32(TDES_INT_RAW_REG_ADDR, 0);
    return SEC_CORE_SUCCESS;
}
 
SEC_ERROR_CODE SEC_init(void)
{
    unsigned int i;

	for(i = 0 ; i < CRYPTO_CHANNEL_MAX_NUM; i++){
        channel_info[i].p_chl_mem_base = CRYPTO_MEM_REGION + i * CRYPTO_MEM_SIZE /3;
        channel_info[i].chl_mem_size = CRYPTO_MEM_SIZE / 3;
        channel_info[i].p_in_buffer_base = channel_info[i].p_chl_mem_base;

        channel_info[i].in_buffer_size = channel_info[i].chl_mem_size / 2;

        channel_info[i].p_out_buffer_base = channel_info[i].p_in_buffer_base + channel_info[i].in_buffer_size;

        channel_info[i].out_buffer_size = channel_info[i].chl_mem_size - channel_info[i].in_buffer_size;


        if (i == 0){ 
            channel_info[i].mode = BLOCK_DMA_MODE; 
        }
        else {
            channel_info[i].mode = STREAMING_MODE; 
        }
        channel_info[i].input_buffer.r_pointer = channel_info[i].p_in_buffer_base;
        channel_info[i].input_buffer.w_pointer = channel_info[i].p_in_buffer_base;
        channel_info[i].output_buffer.r_pointer = channel_info[i].p_out_buffer_base;
        channel_info[i].output_buffer.w_pointer = channel_info[i].p_out_buffer_base;

        channel_info[i].emi = NO_SET;
        channel_info[i].count = 0;
    }

    
    DEBUG_PRINTF("SEC Init\n");
    return SEC_CORE_SUCCESS;
}



SEC_ERROR_CODE SEC_term(void)
{
    scwrite32(TDES_INT_CLS_ADDR, 0xFFFFFFFF);
    scwrite32(TDES_INT_ENB_ADDR, 0);
    scwrite32(TDES_DATA_SIZE(0), 0);


    return SEC_CORE_SUCCESS;
}


SEC_ERROR_CODE PV_read(unsigned int cmd, unsigned int *value)
{

    CHECK_WITH_TMO(scread32(MAILBOX0) & 0x80000000);
	scwrite32(MAILBOX1, cmd);
	scwrite32(MAILBOX7, 0x0);
	scwrite32(MAILBOX0, 0x80000012);
    CHECK_WITH_TMO(!(scread32(MAILBOX7) & 0x80000000));
	if (scread32(MAILBOX7) & 0xffff)
		return SEC_CORE_ERROR;

	*value = scread32(MAILBOX2);
	scwrite32(MAILBOX7, 0x0);
	return SEC_CORE_SUCCESS;
}


static SEC_ERROR_CODE Key_write(unsigned int         cmd, 
                                unsigned int         type,
                                const unsigned char* key,
                                int                  size)
{
    unsigned int * val = (unsigned int *)key;
    int i;

    if (size & 0x3)
        return SEC_CORE_ERROR;  
   

    CHECK_WITH_TMO(scread32(MAILBOX0) & 0x80000000);
	scwrite32(MAILBOX1, type);
    for(i = 0; i < size; i += 4)
    	scwrite32(MAILBOX2 + i, *val++);

	scwrite32(MAILBOX7, 0x0);
	scwrite32(MAILBOX0, cmd);
    CHECK_WITH_TMO(!(scread32(MAILBOX7) & 0x80000000));
	if (scread32(MAILBOX7) & 0xffff)
		return SEC_CORE_ERROR;
	scwrite32(MAILBOX7, 0x0);
	return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Init(unsigned int chl)
{
    if (channel_info[chl].mode == BLOCK_DMA_MODE) {
        scwrite32(SAR_TS(chl), channel_info[chl].p_in_buffer_base);
        scwrite32(DAR_TS0(chl), channel_info[chl].p_out_buffer_base);
        scwrite32(TDES_CONT(chl), 0);
    } else if (channel_info[chl].mode == STREAMING_MODE) {
        scwrite32(SRC_LOW_ADDR_TS(chl), channel_info[chl].p_in_buffer_base);
        scwrite32(SRC_UP_ADDR_TS(chl), channel_info[chl].p_in_buffer_base + channel_info[chl].in_buffer_size);

        scwrite32(SRC_WR_PT_TS(chl), channel_info[chl].input_buffer.w_pointer);
        scwrite32(SRC_RD_PT_TS(chl), channel_info[chl].input_buffer.r_pointer);

        scwrite32(DES_LOW_ADDR_TS(chl), channel_info[chl].p_out_buffer_base);
        scwrite32(DES_UP_ADDR_TS(chl), channel_info[chl].p_out_buffer_base + channel_info[chl].out_buffer_size);

        scwrite32(DES_WR_PT_TS(chl), channel_info[chl].output_buffer.w_pointer);
        scwrite32(DES_RD_PT_TS(chl), channel_info[chl].output_buffer.r_pointer);

        scwrite32(TDES_CONT(chl), 0);
    }
    channel_info[chl].write_count = 0;
    channel_info[chl].read_count = 0;
    DEBUG_PRINTF("CHL Init\n");
    
	return SEC_CORE_SUCCESS;
}


SEC_ERROR_CODE SetHostKey(unsigned int chl, SEC_EMI emi, unsigned char * hostkey, int size)
{
    unsigned int kldf;

    if ((IS_DVB(emi) && (size != 8))   || 
        (IS_TDES(emi) && (size != 16)) ||
        (IS_AES(emi) && (size != 16))  ||
        (chl >= CRYPTO_CHANNEL_MAX_NUM))
        {
            printf("--func:%s line:%d invalid input parameters\n", __FUNCTION__,__LINE__);
            return SEC_CORE_ERROR;
        }
    
    /* get kld status */
    SEC_ASSERT(PV_read(0x2, &kldf));

    if (!kldf) { /* Default mode */
        if (IS_TDES(emi))
            {
                SEC_ASSERT(Key_write(0x80000004, 0x80000100 | chl, hostkey, size));
            }
        else if (IS_AES(emi))
            {
                SEC_ASSERT(Key_write(0x80000004, 0x80000000 | chl, hostkey, size));
            }
        else 
            {
                printf("unsupported emi = 0x%08x\n", emi);
                return SEC_CORE_ERROR;
            }
    } else { /* In Restricted mode */
        if (IS_TDES(emi))
            {
                unsigned int Key0 = 0;
                unsigned int Key1 = 0;
                unsigned int Key2 = 0;
                unsigned int Key3 = 0;
                unsigned int KeyReg0;
                unsigned int KeyReg1;

                Key0 = *(unsigned int *)(hostkey+0x0);
                Key1 = *(unsigned int *)(hostkey+0x4);
                Key2 = *(unsigned int *)(hostkey+0x8);
                Key3 = *(unsigned int *)(hostkey+0xc);

                KeyReg0 = KeyReg1 = 0;
                KeyReg0 = ((Key1 & 0xfe000000) >> (32-7)) |
                    ((Key1 & 0x00fe0000) >> (24-14)) |
                    ((Key1 & 0x0000fe00) << (14-9)) |
                    ((Key1 & 0x000000fe) << (21-1)) |
                    ((Key0 & 0x1e000000) << (3));

                KeyReg1 = ((Key0 & 0xe0000000) >> 29) |
                    ((Key0 & 0x00fe0000) >> 14) |
                    ((Key0 & 0x0000fe00) << 1) |
                    ((Key0 & 0x000000fe) << 16);

                scwrite32(TDES_KEY0(chl), KeyReg0);
                scwrite32(TDES_KEY1(chl), KeyReg1);
                scwrite32(TDES_KEY4(chl), KeyReg0);
                scwrite32(TDES_KEY5(chl), KeyReg1);

                KeyReg0 = KeyReg1 = 0;
                KeyReg0 = ((Key3 & 0xfe000000) >> (32-7)) |
                    ((Key3 & 0x00fe0000) >> (24-14)) |
                    ((Key3 & 0x0000fe00) << (14-9)) |
                    ((Key3 & 0x000000fe) << (21-1)) |
                    ((Key2 & 0x1e000000) << (3));

                KeyReg1 = ((Key2 & 0xe0000000) >> 29) |
                    ((Key2 & 0x00fe0000) >> 14) |
                    ((Key2 & 0x0000fe00) << 1) |
                    ((Key2 & 0x000000fe) << 16);

                scwrite32(TDES_KEY2(chl), KeyReg0);
                scwrite32(TDES_KEY3(chl), KeyReg1);
            }
        else if (IS_AES(emi)){
            scwrite32(TDES_KEY3(chl), scSWAP(*(unsigned int *)(hostkey+0x0)));
            scwrite32(TDES_KEY2(chl), scSWAP(*(unsigned int *)(hostkey+0x4)));
            scwrite32(TDES_KEY1(chl), scSWAP(*(unsigned int *)(hostkey+0x8)));
            scwrite32(TDES_KEY0(chl), scSWAP(*(unsigned int *)(hostkey+0xc)));
        }  else 
            {
                printf("unsupported emi = 0x%08x\n", emi);
                return SEC_CORE_ERROR;
            }
    }
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE SetCryptoMode(unsigned int          chl,
                             SEC_EMI               emi,
                             unsigned char *       Ivector,
                             int                   size)
{    
    //    unsigned char crm, cim, sbm = 0, ivf = 0, enc = 0;
    unsigned int  temp;
    unsigned int refreshIv =1;
    if (chl >= CRYPTO_CHANNEL_MAX_NUM) {
        printf( "invalid channel number!\n");
        return SEC_CORE_INVALID_PARAM;
    }
    
    if (!IS_ECB(emi) && refreshIv &&
        ( (Ivector == NULL) ||
          (IS_AES(emi) && (size != 16)) ||
          (IS_TDES(emi) && (size != 8)) ) )
        {
            printf("invalid input IV size or vector:emi=0x%x, size=%d \n", emi, size);
            return SEC_CORE_INVALID_PARAM;
        }
    scwrite32(TDES_COM(chl), 0 );    

	/* Step 2: set IV vector */
	if (!IS_ECB(emi) && refreshIv) { /* CBC */
        if (size == 8){ 
            scwrite32(TDES_IV1(chl), scSWAP(*(unsigned int *)(Ivector)));
			scwrite32(TDES_IV0(chl), scSWAP(*(unsigned int *)(Ivector+4)));
			scwrite32(TDES_IV2(chl), 0);
			scwrite32(TDES_IV3(chl), 0);

        }
        else // size == 16
        {
    		scwrite32(TDES_IV3(chl), scSWAP(*(unsigned int *)(Ivector)));
			scwrite32(TDES_IV2(chl), scSWAP(*(unsigned int *)(Ivector+4)));
			scwrite32(TDES_IV1(chl), scSWAP(*(unsigned int *)(Ivector+8)));
			scwrite32(TDES_IV0(chl), scSWAP(*(unsigned int *)(Ivector+12)));
            
        }
	}
    //    temp = scread32(TDES_COM(chl)) & (~0x7ff);


    temp = ((emi & 0xff) <<9) | 
        (((emi & 0x700) >>8) << 6) | 
        (((emi & 0xf0000) >> 16) <<2) |
        ((emi & 0x80000000)>> 30) |
        (refreshIv & 0x1);

    scwrite32(TDES_COM(chl), temp);    

    DEBUG_PRINTF("SEC Set Mode:emi=0x%x, temp=0x%x\n",emi, temp);
    DEBUG_PRINTF("SEC Mode:0x%x\n",scread32(TDES_COM(chl)));
    return SEC_CORE_SUCCESS;  
}

SEC_ERROR_CODE GetCryptoIV(unsigned int          chl,
                           SEC_EMI               emi,
                           unsigned char *       Ivector,
                           int *                 size)
{
    if (size == NULL || *size == 0)
        return SEC_CORE_INVALID_PARAM;
    if (IS_ECB(emi) && ((Ivector == NULL) ||
                        (IS_AES(emi) && (*size < 16)) ||
                        (IS_TDES(emi) && (*size < 8)) ) )    
        {
            return SEC_CORE_INVALID_PARAM;
        }

    if (IS_AES(emi))
        *size = 16;
    else if (IS_TDES(emi))
        *size =8;

	if (!IS_ECB(emi)){ /* CBC */
            if (IS_TDES(emi)){
                *size = 8; 
                (*(unsigned int *)(Ivector)) = scSWAP(scread32(TDES_IV1(chl)));
                (*(unsigned int *)(Ivector+4)) = scSWAP(scread32(TDES_IV0(chl)));

            }
            else if (IS_AES(emi)){// size == 16
 
            *size = 16;
    		(*(unsigned int *)(Ivector))= scSWAP(scread32(TDES_IV3(chl)));
    		(*(unsigned int *)(Ivector+4))= scSWAP(scread32(TDES_IV2(chl)));
    		(*(unsigned int *)(Ivector+8))= scSWAP(scread32(TDES_IV1(chl)));
    		(*(unsigned int *)(Ivector+12))= scSWAP(scread32(TDES_IV0(chl)));

        }
	}    
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Start(unsigned int chl)
{

    if (chl >= CRYPTO_CHANNEL_MAX_NUM) {
        printf( "invalid channel number!\n");
        return SEC_CORE_INVALID_PARAM;
    }

    //    scwrite32(TDES_CONT(chl), 0);    
    while (((scread32(TDES_COM(chl)) >> 30) & 0x1) == 0x1 ){
       printf("Not Stop!\n");
    }

    DEBUG_PRINTF("TDES_COM(%d) = 0x%x\n", chl, scread32(TDES_COM(chl)));
    scwrite32(TDES_COM(chl), (scread32(TDES_COM(chl)) | 0x80000000));
    channel_info[chl].write_count = 0;
    channel_info[chl].read_count = 0;
    channel_info[chl].term_last_flag = 0;
    channel_info[chl].input_buffer.r_pointer = channel_info[chl].p_in_buffer_base;
    channel_info[chl].input_buffer.w_pointer = channel_info[chl].p_in_buffer_base;
    channel_info[chl].output_buffer.r_pointer = channel_info[chl].p_out_buffer_base;
    channel_info[chl].output_buffer.w_pointer = channel_info[chl].p_out_buffer_base;


    scwrite32(SRC_WR_PT_TS(chl), channel_info[chl].input_buffer.w_pointer);
    scwrite32(SRC_RD_PT_TS(chl), channel_info[chl].input_buffer.r_pointer);

    scwrite32(DES_WR_PT_TS(chl), channel_info[chl].output_buffer.w_pointer);
    scwrite32(DES_RD_PT_TS(chl), channel_info[chl].output_buffer.r_pointer);
    DEBUG_PRINTF("SEC Chl %d start\n", chl);
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Stop(unsigned int chl)
{

    if (chl >= CRYPTO_CHANNEL_MAX_NUM) {
        printf( "invalid channel number!\n");
        return SEC_CORE_INVALID_PARAM;
    }    
    scwrite32(TDES_COM(chl), (scread32(TDES_COM(chl)) & ~0x80000000));

    scwrite32(TDES_CONT(chl), 0);
    channel_info[chl].write_count = 0;
    channel_info[chl].read_count = 0;
    channel_info[chl].term_last_flag = 0;
    DEBUG_PRINTF("SEC Chl %d stop\n", chl);
    return SEC_CORE_SUCCESS;

}

SEC_ERROR_CODE CHL_Enable_In_Interrupt(unsigned int chl)
{
    unsigned int value;
    if (channel_info[chl].mode == STREAMING_MODE){
        value = scread32(TDES_INT_ENB_ADDR);
        value |= (1 << (4 * chl));
        scwrite32(TDES_INT_ENB_ADDR, value);
    }

    DEBUG_PRINTF("SEC Chl %d Interrupt enable\n", chl);
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Set_In_Threshold(unsigned int chl, unsigned int in_threshold)
{
    if (channel_info[chl].mode == STREAMING_MODE){
        scwrite32(SRC_THRESHOLD_TS(chl), in_threshold);
    }
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Set_Out_Threshold(unsigned int chl, unsigned int out_threshold)
{

    if (channel_info[chl].mode == STREAMING_MODE){
        scwrite32(SRC_THRESHOLD_TS(chl), out_threshold);
    }
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Enable_Out_Interrupt(unsigned int chl)
{
    unsigned int vale;
    if (channel_info[chl].mode == STREAMING_MODE){
       vale = scread32(TDES_INT_ENB_ADDR);
       vale |= (1 << (4 * chl + 1));
        scwrite32(TDES_INT_ENB_ADDR, vale);
    }
    return SEC_CORE_SUCCESS;
}


SEC_ERROR_CODE CHL_Enable_End_Interrupt(unsigned int chl)
{
    unsigned int vale;
    vale = scread32(TDES_INT_ENB_ADDR);
    if (chl == 0) 
        vale |= 0x1;
    else 
        vale |= (1 << (4 * chl + 2));
    scwrite32(TDES_INT_ENB_ADDR, vale);
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Disable_In_Interrupt(unsigned int chl)
{
    unsigned int vale;
    if (channel_info[chl].mode == STREAMING_MODE){
        vale = scread32(TDES_INT_ENB_ADDR);
        vale &=~( 1 << 4 * chl);
        scwrite32(TDES_INT_ENB_ADDR, vale);
    }
    return SEC_CORE_SUCCESS;
}

SEC_ERROR_CODE CHL_Disable_Out_Interrupt(unsigned int chl)
{
    unsigned int vale;
    if (channel_info[chl].mode == STREAMING_MODE){
        vale = scread32(TDES_INT_ENB_ADDR);
        vale &=~( 1 << (4 * chl +1) );
        scwrite32(TDES_INT_ENB_ADDR, vale);
    }
    return SEC_CORE_SUCCESS;
}


SEC_ERROR_CODE CHL_Disable_End_Interrupt(unsigned int chl)
{
   unsigned int vale;
    vale = scread32(TDES_INT_ENB_ADDR);
    if (chl == 1) 
        vale &= ~0x1;
    else 
        vale &= ~(1 << (4 * chl + 2));
    scwrite32(TDES_INT_ENB_ADDR, vale);
    return SEC_CORE_SUCCESS;
}


/** 
 * Flush the rest data for streaming mode
 * 
 * @param chl: channel number 
 * 
 * @return 
 */

SEC_ERROR_CODE CHL_Flush(unsigned int chl)
{
    unsigned int in_wp;
    unsigned int free_size;
    unsigned int term_data;
    unsigned int written_size;

    if (chl >= CRYPTO_CHANNEL_MAX_NUM) {
        printf( "invalid channel number!\n");
        return SEC_CORE_INVALID_PARAM;
    }    
    if (channel_info[chl].mode != STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    }
    in_wp = (channel_info[chl].input_buffer.w_pointer) & 0x7FFFFFFF;

    /* workaround for bug */
    if (((in_wp % 64) != 0) || channel_info[chl].write_count < 64){
        scwrite32(TDES_CONT(chl), 1); 
    } else {
        do{
            free_size =CHL_Check_InBuffer_FreeSize(chl);
            DEBUG_PRINTF("Flush: free data =%d\n",free_size);
        }while( free_size <4 );
        term_data = 0xbeefbeef;

        written_size = Stream_CHL_Write_Data(chl,(char *) &term_data, 4);
        channel_info[chl].write_count -= written_size;
        scwrite32(TDES_CONT(chl), 1); 
    }

    DEBUG_PRINTF("SEC Chl %d Flush\n", chl);
    return SEC_CORE_SUCCESS;
}


/** Set Block Mode channel access mode: DMA/HOST
 *  Only for Block mode channel
 * 
 * @param chl: channel number: now only support channel 0 
 * @param mode: DMA or HOST
 * 
 * @return 
 */
SEC_ERROR_CODE SetBlockCHLMode(unsigned int chl, SEC_CHANNEL_MODE mode)
{
    if (chl >= CRYPTO_CHANNEL_MAX_NUM) {
        printf( "invalid channel number!\n");
        return SEC_CORE_INVALID_PARAM;
    }    
    if (channel_info[chl].mode == STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    } else if (mode == BLOCK_DMA_MODE) {
        scwrite32(TDES_CONT(chl), 0);
    } else {
        scwrite32(TDES_CONT(chl), 1);
    } 
    channel_info[chl].mode = mode;
    DEBUG_PRINTF("SEC Chl %d Mode to %d\n", chl, mode);
    return SEC_CORE_SUCCESS;

}

/** 
 * 
 * 
 * @param chl : chl number
 * @param status:  0: for stopped  1: in processing 
 * 
 * @return 
 */
SEC_ERROR_CODE GetCryptoDone(unsigned int chl, unsigned char* status)
{
    unsigned int raw_int_value;
    unsigned int int_value;

    if (chl >= CRYPTO_CHANNEL_MAX_NUM || status == NULL) {
        return SEC_CORE_INVALID_PARAM;
    }
    raw_int_value =scread32(TDES_INT_RAW_REG_ADDR);
    int_value = scread32(TDES_INT_REG_ADDR);



    if (chl == 0) 
        *status = (~raw_int_value) & 0x1;
    else 
        *status = ((~raw_int_value) >> (4 * chl + 2)) & 0x1;

    DEBUG_PRINTF("COM=0x%x, CON=0x%x\n", scread32(TDES_COM(chl)), scread32(TDES_CONT(chl))); 
    DEBUG_PRINTF("raw=0x%x, status=%d\n", raw_int_value, *status);     
    if (*status == 0){
        
        scwrite32(TDES_CONT(chl), 0);
        
        if (chl == 0) 
            raw_int_value = (raw_int_value) & (~0x1);
        else 
            raw_int_value = (raw_int_value) & (~(1 << (4 * chl + 2)));

        scwrite32(TDES_INT_RAW_REG_ADDR, raw_int_value);
    }
    

    return SEC_CORE_SUCCESS;    

}

SEC_ERROR_CODE Update_CHL_InWr_Pointer(unsigned int chl)
{
    if (chl >= CRYPTO_CHANNEL_MAX_NUM || channel_info[chl].mode != STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    }
    scwrite32(SRC_WR_PT_TS(chl),channel_info[chl].input_buffer.w_pointer);
    DEBUG_PRINTF("SEC Chl %d update In write Pointer to 0x%x\n", chl, channel_info[chl].input_buffer.w_pointer);
    return SEC_CORE_SUCCESS;    
}

SEC_ERROR_CODE Get_CHL_InRe_Pointer(unsigned int chl) //b
{ 
   if (chl >= CRYPTO_CHANNEL_MAX_NUM || channel_info[chl].mode != STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    }
   channel_info[chl].input_buffer.r_pointer = scread32(SRC_RD_PT_TS(chl));
   return SEC_CORE_SUCCESS;    
}

SEC_ERROR_CODE Update_CHL_OutRe_Pointer(unsigned int chl)
{
    if (chl >= CRYPTO_CHANNEL_MAX_NUM || channel_info[chl].mode != STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    }
    scwrite32(DES_RD_PT_TS(chl),channel_info[chl].output_buffer.r_pointer);
    DEBUG_PRINTF("SEC Chl %d update out read Pointer to 0x%x \n", chl, channel_info[chl].output_buffer.r_pointer);
    return SEC_CORE_SUCCESS;    
}

SEC_ERROR_CODE Get_CHL_OutWr_Pointer(unsigned int chl)
{
    if (chl >= CRYPTO_CHANNEL_MAX_NUM || channel_info[chl].mode != STREAMING_MODE) {
        return SEC_CORE_INVALID_PARAM;
    }

    channel_info[chl].output_buffer.w_pointer = scread32(DES_WR_PT_TS(chl));
    return SEC_CORE_SUCCESS;    
}

int CHL_Check_OutBuffer_DataSize(unsigned int chl)
{
    int len;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;
    unsigned int diff;

    if (channel_info[chl].mode != STREAMING_MODE) {
        len = (int)scread32(TDES_DATA_SIZE(chl));
        if (len == 0) return channel_info[chl].count;
        else return 0;
    }

    Get_CHL_OutWr_Pointer(chl);
    
	wp = channel_info[chl].output_buffer.w_pointer;
	rp = channel_info[chl].output_buffer.r_pointer;

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;

	if (wp_tog == rp_tog) {
		len = wp - rp;
	}
	else {
		len = channel_info[chl].out_buffer_size + wp - rp;
	}
    diff = (unsigned int)(channel_info[chl].write_count - channel_info[chl].read_count);
    if (len > diff) 
        len = diff;
    DEBUG_PRINTF("SEC Chl %d check output data size: %d(wp=0x%x,rp=0x%x )\n", chl, len,wp,rp);
	return len;

}

int CHL_Check_InBuffer_FreeSize(unsigned int chl)
{

    int len;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;
    Get_CHL_InRe_Pointer(chl);
    
	wp = channel_info[chl].input_buffer.w_pointer;
	rp = channel_info[chl].input_buffer.r_pointer;

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;

	if (wp_tog == rp_tog) {
		len = channel_info[chl].in_buffer_size + rp - wp;
	}
	else {
		len = rp - wp;
	}

	return len;

}

int Block_CHL_Read_Data(unsigned int chl, char* buf, unsigned int len)
{
    unsigned int read_addr;

    if (len > channel_info[chl].out_buffer_size) {
        printf("Crypto Error: Block CHL read size too large\n");
        return -1;
    }

    if (chl >= CRYPTO_CHANNEL_MAX_NUM || buf == NULL) {
        return -1;
    }

    if (len == 0 )  return 0;
    read_addr = channel_info[chl].p_out_buffer_base;
    
    memcpy(buf, (void *) read_addr, len);
    return len;
}


int Stream_CHL_Read_Data(unsigned int chl, char * buf, unsigned int len)
{
    int data_size = 0;
    unsigned int temp_size = 0;
    unsigned int read_addr;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;

    unsigned int diff;

    if (chl >= CRYPTO_CHANNEL_MAX_NUM || buf == NULL) {
        return -1;
    }

    if (len == 0 )  return 0;

    if (channel_info[chl].write_count <= channel_info[chl].read_count)
        return 0;

    diff = (unsigned int)(channel_info[chl].write_count - channel_info[chl].read_count);
    if (len > diff) 
        len = diff;

    Get_CHL_OutWr_Pointer(chl);
	wp = channel_info[chl].output_buffer.w_pointer;
	rp = channel_info[chl].output_buffer.r_pointer;

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;
    read_addr = rp;         
	if (wp_tog != rp_tog) {
		temp_size = channel_info[chl].p_out_buffer_base + channel_info[chl].out_buffer_size - rp;
        if (temp_size > len) 
            temp_size = len;
       
        memcpy((void *)buf, (void *)read_addr, temp_size);

        len -= temp_size;
        rp = rp + temp_size;
        data_size += temp_size;
        if (rp >= channel_info[chl].p_out_buffer_base + channel_info[chl].out_buffer_size) {
            rp -= channel_info[chl].out_buffer_size;
            rp_tog = ~rp_tog;
        }
        read_addr = rp;        
        buf += temp_size;
	}

    if (len > 0) {
        temp_size = wp - rp;
        if (temp_size > len) 
            temp_size = len;

        memcpy((void *)buf, (void *)read_addr, temp_size);
        data_size += temp_size;
        rp += temp_size;
        if (rp >= channel_info[chl].p_out_buffer_base + channel_info[chl].out_buffer_size) {
            rp -= channel_info[chl].out_buffer_size;
            rp_tog = ~rp_tog;
        }
    }
    channel_info[chl].output_buffer.r_pointer = (rp & 0x7fffffff) | (rp_tog & 0x80000000);

    Update_CHL_OutRe_Pointer(chl);
    DEBUG_PRINTF("SEC Chl %d streaming read data: 0x%x\n", chl, data_size);
    channel_info[chl].read_count += data_size;
    return data_size;

}

int Stream_CHL_Write_Data(unsigned int chl, char * buf, unsigned int len)
{
    int data_size = 0;
    unsigned int temp_size = 0;
    unsigned int write_addr;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;

    if (chl >= CRYPTO_CHANNEL_MAX_NUM || buf == NULL) {
        return -1;
    }

    if (len == 0 )  return 0;

    Get_CHL_InRe_Pointer(chl);
    
	wp = channel_info[chl].input_buffer.w_pointer;
	rp = channel_info[chl].input_buffer.r_pointer;

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;
    write_addr = wp;        
	if (wp_tog == rp_tog) {
		temp_size = channel_info[chl].p_in_buffer_base + channel_info[chl].in_buffer_size - wp;
        if (temp_size > len) 
            temp_size = len;

        memcpy((void *)write_addr, buf, temp_size);

        len -= temp_size;
        wp  += temp_size;
        data_size += temp_size;
        if (wp >= channel_info[chl].p_in_buffer_base + channel_info[chl].in_buffer_size) {
            wp -= channel_info[chl].in_buffer_size;
            wp_tog = ~wp_tog;
        }
        write_addr = wp;
        buf += temp_size;
	}

    if (len > 0) {
        temp_size = rp - wp;
        if (temp_size > len) 
            temp_size = len;

        memcpy((void *)write_addr, buf, temp_size);
        data_size += temp_size;
        wp += temp_size;
        if (wp >= channel_info[chl].p_in_buffer_base + channel_info[chl].in_buffer_size) {
            wp -= channel_info[chl].in_buffer_size;
            wp_tog = ~wp_tog;
        }
    }
    channel_info[chl].input_buffer.w_pointer = (wp & 0x7fffffff) | (wp_tog & 0x80000000);
    Update_CHL_InWr_Pointer(chl);
    channel_info[chl].write_count += data_size;
    DEBUG_PRINTF("SEC Chl %d streaming write data: 0x%x\n", chl, data_size);
    
    return data_size;

}

int Block_CHL_Write_Data(unsigned int chl,  char * buf, unsigned int len)
{
    unsigned int write_addr;

    if (len > channel_info[chl].in_buffer_size) 
        return -1;
    if (len == 0 )  return 0;
    
    write_addr = channel_info[chl].p_in_buffer_base;

    memcpy((void *) write_addr, buf, len);
    scwrite32(TDES_DATA_SIZE(chl), (unsigned int)len);
    channel_info[chl].count = len;
    DEBUG_PRINTF("SEC Chl %d block write data: 0x%x\n", chl, len);
    return len;

}
