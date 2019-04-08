#include <common.h>
#include <command.h>

#include "usb_update.h"

#define MUSB_HSDMA_CHANNELS		2

#define MGC_Read8(_pBase, _offset) *(volatile uint8_t*)(_pBase + _offset)
#define MGC_Read16(_pBase, _offset) *(volatile uint16_t*)(_pBase + _offset)
#define MGC_Read32(_pBase, _offset) *(volatile uint32_t*)(_pBase + _offset)

#define MGC_Write8(_pBase, _offset, _data) MGC_Read8(_pBase, _offset) = (uint8_t)_data
#define MGC_Write16(_pBase, _offset, _data) MGC_Read16(_pBase, _offset) = (uint16_t)_data
#define MGC_Write32(_pBase, _offset, _data) MGC_Read32(_pBase, _offset) = (uint32_t)_data

#define MGC_EXTRA_INIT()  do { } while(0)

static unsigned char *BASE_ADDR = (unsigned char *)0x10200000;
static unsigned char get_desc_cmd[] = { 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00 };
static unsigned char dev_descriptor[18] = { 0 };

extern int do_auto_update(void);

unsigned char MGC_HdrcReadUlpiReg(unsigned char* pBase, unsigned char bAddr, unsigned char* pbData)
{
    unsigned char bCtl = 0;

    /* ensure not powered down */
    if(MGC_Read8(pBase, 0x01) & 0x01) {
		return -1;
    }

    /* polled */
    MGC_Write8(pBase, 0x75, bAddr);
    MGC_Write8(pBase, 0x76, 0x04 | 0x01);
    while(!(0x02 & bCtl)) {
		bCtl = MGC_Read8(pBase, 0x76);
    }

    *pbData = MGC_Read8(pBase, 0x74);
    MGC_Write8(pBase, 0x76, 0);

    return 0;
}

unsigned char MGC_HdrcWriteUlpiReg(unsigned char* pBase, unsigned char bAddr, unsigned char bData)
{
    unsigned char bCtl = 0;

printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
    /* ensure not powered down */
    if(MGC_Read8(pBase, 0x01) & 0x01) {
		return -1;
    }
printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);

    /* polled */
    MGC_Write8(pBase, 0x75, bAddr);
    MGC_Write8(pBase, 0x74, bData);
    MGC_Write8(pBase, 0x76, 0x01);
printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);

    while(!(0x02 & bCtl)) {
printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
		bCtl = MGC_Read8(pBase, 0x76);
    }

    MGC_Write8(pBase, 0x76, 0);
printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);

    return -1;
}

#if 0 //not used.
static void MGC_Ulpi_sta_read(unsigned char* pThis)
{
	unsigned int tmp = 0, addr = 0;

	//printf("Enter %s:%d ... ... \n", __FUNCTION__, __LINE__);
//	MGC_Write8(pThis, 0x70, 0x03);
	//printf(" DELME... ... \n");

       addr = ULPI_OTG_CTL;
       MGC_HdrcReadUlpiReg(pThis, addr, (unsigned char*) &tmp);
       //printf (" **0X%X read from ULPI ULPI_OTG_CTL reg(0X%X)\n", tmp,addr);

       addr = ULPI_FUN_CTL;
       MGC_HdrcReadUlpiReg(pThis, addr, (unsigned char*) &tmp);
       //printf (" **0X%X read from ULPI ULPI_FUN_CTL reg(0X%X)\n", tmp,addr);

       addr = ULPI_IF_CTL;
       MGC_HdrcReadUlpiReg(pThis, addr,(unsigned char*)  &tmp);
       //printf (" **0X%X read from ULPI ULPI_IF_CTL reg(0X%X)\n", tmp,addr);

       addr = ULPI_DEBUG;
       MGC_HdrcReadUlpiReg(pThis, addr,(unsigned char*)  &tmp);
       //printf (" **0X%X read from ULPI ULPI_DEBUG reg(0X%X)\n", tmp,addr);

	//printf("%s() ok \n", __FUNCTION__);

	return;
}
#endif

static void MGC_Ulpi_setting(unsigned char* pThis)
{
	//unsigned int tmp = 0;
    //unsigned int addr = 0;

	//printf("Enter %s:%d ... ... \n", __FUNCTION__, __LINE__);
	MGC_Write8(pThis, 0x70, 0x01);
	//printf(" ONLY SET ULPI_VBUS 0x01\n");
#if 0
	addr = ULPI_FUN_CTL+1; /* write the function RESET regs, or with the write data */
	tmp = 0x20;
        printf(" RESET PHY\n");
	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
#endif
	udelay(50000);

#if 0
	tmp = MGC_Read8(pThis, REG_LPI_VBUS);

	/* set the DRV_VBUS_EXT = 1 */
	addr = ULPI_OTG_CTL;
	MGC_HdrcReadUlpiReg(pThis, addr, (unsigned char*) &tmp);
	tmp |= 0x40;
	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);

	/* set the USEExternalVbusIndicator = 1 */
	MGC_HdrcReadUlpiReg(pThis, addr, (unsigned char*) &tmp);
	tmp |= 0x80;
	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);

        printf("SET ULPI_OTG_CTL DRV_VBUS DRV_VBUS_EXT  0x03\n");
        addr = ULPI_OTG_CTL;
        MGC_HdrcReadUlpiReg(pThis, addr,(unsigned char*)  &tmp);
        tmp |= 0x40;
        MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
	/* set the DRV_VBUS = 1, Turn on the PHY led */
	MGC_HdrcReadUlpiReg(pThis, addr,(unsigned char*)  &tmp);
	tmp |= 0x20;
	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);

        printf("AFTER SET ULPI_OGT_CTL,WAIT,PRESS ANY KEY TO CONTINUE\n");
        getc();

	/* set the DPpulldown and Dmpulldown = 11 */
//       addr = ULPI_OTG_CTL;
//	MGC_HdrcReadUlpiReg(pThis, addr, &tmp);
//	tmp |= 0x06;
//	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
//
//	/* set the TermSelect = 1 */
//	addr = ULPI_FUN_CTL;
//	MGC_HdrcReadUlpiReg(pThis, addr, &tmp);
//	tmp |= 0x04;
//	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
//
//	/* set the OP Mode = 0 */
//	MGC_HdrcReadUlpiReg(pThis, addr, &tmp);
//	tmp |= 0xe7;
//	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
//
//	/* set the XcvrSelect[0] = 1 */
//	MGC_HdrcReadUlpiReg(pThis, addr, &tmp);
//	tmp |= 0x01;
//	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
//
//	/* set the Indicator Pass Thru  = 1  and set Indicator Complement =0 */
#if 0
	addr = ULPI_IF_CTL;
	MGC_HdrcReadUlpiReg(pThis, addr,(unsigned char*)  &tmp);
	tmp |= 0x40;
	tmp &= 0xdf;
	MGC_HdrcWriteUlpiReg(pThis, addr, tmp);
#endif
        udelay(1000);
	printf("%s() ok \n", __FUNCTION__);

#endif

	return;
}

/* Read a word over the MCU interface and check it matches the expected value */
void MGC_CheckReg(unsigned int addr,unsigned int exp, unsigned int be ,unsigned int temp)
{
	unsigned int ret;
	unsigned int readout=0;
	unsigned int readexp;

	unsigned char b0=0, b1=0, b2=0, b3=0;

	switch(be) {
		case 1:
		{
			b0=1;
			readout = MGC_Read8(BASE_ADDR, addr);
			break;
		}
		case 2:
		{
			b1=1;
			readout = MGC_Read8(BASE_ADDR, addr) << 8;
			break;
		}
		case 4:
		{
			b2=1;
			readout = MGC_Read8(BASE_ADDR, addr) << 16;
			break;
		}
		case 8:
		{
			b3=1;
			readout = MGC_Read8(BASE_ADDR, addr) << 24;
			break;
		}
		case 3:
		{
			b0=1; b1=1;
			readout = MGC_Read16(BASE_ADDR, addr);
			break;
		}
		case 12:
		{
			b2=1; b3=1;
			readout = MGC_Read16(BASE_ADDR, addr) << 16;
			break;
		}
		case 15:
		{
			b0=1; b1=1; b2=1; b3=1;
			readout = MGC_Read32(BASE_ADDR, addr);
			break;
		}
	}

	ret = readout % 256;
	readexp = exp % 256;

	if (b0==1 && (ret != readexp)) {
		printf (" **ERROR the 0 byte: 0X%X read from reg (0X%X), (expected 0X%X) \n", ret,addr,exp);
	}

	ret = readout / 256;
	ret = ret % 256;
	readexp = exp / 256;
	readexp = readexp % 256;

	if (b1==1 && (ret != readexp)) {
		printf (" **ERROR the 1 byte: 0X%X read from reg (0X%X), (expected 0X%X) \n", ret,addr,exp);
	}

	ret = readout / 65536;
	ret = ret % 256;
	readexp = exp / 65536;
	readexp = readexp % 256;

	if (b2==1 && (ret != readexp)) {
		printf (" **ERROR the 2 byte: 0X%X read from reg(0X%X), (expected 0X%X) \n", ret,addr,exp);
	}

	ret = readout / 0x1000000;
	ret = ret % 256;
	readexp = exp / 0x1000000;
	readexp = readexp % 256;


	if (b3==1 && (ret != readexp)) {
		printf (" **ERROR the 3 byte: 0X%X read from reg (0X%X), (expected 0X%X) \n", ret,addr,exp);
	}

	return;
}

void sleep(unsigned int loopi)
{
	unsigned int loopk;
	unsigned int loopx = loopi * 10;

	for (loopk = 0; loopk <= loopx; loopk++) {
		for (loopk = 0; loopk <= loopx; loopk++) {
                  udelay(500); /* nothing to do here */
		}
	}

	return;
}

int MGC_CheckIntr(int intr_addr, unsigned int intr_data,unsigned int be)
{

	int iloop = 0;
        int tmp =0;
	unsigned int readout = 0;

	switch(intr_addr)
	{
		case REG_INTRTX:
		{
			do {
				readout = MGC_Read16(BASE_ADDR, intr_addr);
				//printf("Read interrupt for (0x%x)\n", intr_addr);
				udelay(100000);
			} while(readout==0x0 && iloop++ < 0x5);

			if (readout == 0x0) {
				printf(" ERROR : No interrupt for (0x%x)\n", intr_addr);
				return -1;
			}
			else {
				if(readout == intr_data) {
					readout = intr_data;
					//printf(" Success :(0x%x)interrupt for (0x%x) detected \n",readout,intr_addr);
				}
				else {
					printf(" ERROR : interrupt data is wrong for (0x%x), Expected:0x%x ,Readout :0x%x \n", intr_addr, intr_data, readout);
					return -1;
				}

				/*switch (readout & 0xffff) {
					case INTR_EP0: printf ("  EP0 interrupt   \n");			break;
					case INTR_EP1: printf ( "  EP1 Tx interrupt   \n");     break;
					case INTR_EP2: printf ( "  EP2 Tx interrupt   \n");     break;
					case INTR_EP3: printf ( "  EP3 Tx interrupt   \n");     break;
					case INTR_EP4: printf ( "  EP4 Tx interrupt   \n");     break;
					case INTR_EP5: printf ( "  EP5 Tx interrupt   \n");     break;
					case INTR_EP6: printf ( "  EP6 Tx interrupt   \n");     break;
					case INTR_EP7: printf ( "  EP7 Tx interrupt   \n");     break;
					case INTR_EP8: printf ( "  EP8 Tx interrupt   \n");     break;
					case INTR_EP9: printf ( "  EP9 Tx interrupt   \n");     break;
					case INTR_EP10: printf ( "  EP10 Tx interrupt   \n");   break;
					case INTR_EP11: printf ( "  EP11 Tx interrupt   \n");   break;
					case INTR_EP12: printf ( "  EP12 Tx interrupt   \n");   break;
					case INTR_EP13: printf ( "  EP13 Tx interrupt   \n");   break;
					case INTR_EP14: printf ( "  EP14 Tx interrupt   \n");   break;
					case INTR_EP15: printf ( "  EP15 Tx interrupt   \n");   break;

					default: printf ( "  Multiple Tx interrupts   \n");  	break;
				}    */
			}

			break;
		}

		case REG_INTRRX:
		{
			do {
				readout = MGC_Read16(BASE_ADDR, intr_addr);
				//printf("Read interrupt for (0x%x)\n", intr_addr);
			        udelay(1000000);
			} while(readout==0x0 && iloop++ < 0x5);

			if (readout == 0x0) {
				printf(" ERROR : No interrupt for (0x%x)\n", intr_addr);
				return -1;
			}
			else {
				if(readout == intr_data) {
					readout=intr_data;
					//printf(" Success :(0x%x)interrupt for (0x%x) detected \n",readout, intr_addr);
				}
				else
				{
					printf(" ERROR : interrupt data is wrong for (0x%x), Expected:0x%x ,Readout :0x%x \n", intr_addr, intr_data, readout);
					return -1;
				}

				/*switch(readout & 0xffff)
				{
					case INTR_EP1: printf ( "  EP1 Rx interrupt   \n");     break;
					case INTR_EP2: printf ( "  EP2 Rx interrupt   \n");     break;
					case INTR_EP3: printf ( "  EP3 Rx interrupt   \n");     break;
					case INTR_EP4: printf ( "  EP4 Rx interrupt   \n");     break;
					case INTR_EP5: printf ( "  EP5 Rx interrupt   \n");     break;
					case INTR_EP6: printf ( "  EP6 Rx interrupt   \n");     break;
					case INTR_EP7: printf ( "  EP7 Rx interrupt   \n");     break;
					case INTR_EP8: printf ( "  EP8 Rx interrupt   \n");     break;
					case INTR_EP9: printf ( "  EP9 Rx interrupt   \n");     break;
					case INTR_EP10: printf ( "  EP10 Rx interrupt   \n");   break;
					case INTR_EP11: printf ( "  EP11 Rx interrupt   \n");   break;
					case INTR_EP12: printf ( "  EP12 Rx interrupt   \n");   break;
					case INTR_EP13: printf ( "  EP13 Rx interrupt   \n");   break;
					case INTR_EP14: printf ( "  EP14 Rx interrupt   \n");   break;
					case INTR_EP15: printf ( "  EP15 Rx interrupt   \n");   break;
					default: printf ( "  Multiple Rx interrupts   \n");  	break;
				}*/
			}

			break;
		}
		case  REG_INTRUSB:
		{
			LOOP:
			do {
				readout = MGC_Read8(BASE_ADDR, intr_addr);
				//printf("Read interrupt for (0x%x): 0x%x\n", intr_addr, readout);
				udelay (500000);
			} while(readout==0x0 && iloop++ < 0x5);

			if (readout==0x0) {
				printf(" ERROR : No interrupt for (0x%x) \n",intr_addr);
				return -1;
			}
			else {
				if(readout==intr_data)
				{
					readout=intr_data;
					//printf(" Success :(0x%x)interrupt for (0x%x) detected \n",readout, intr_addr);
				}
				else {
					printf(" ERROR : interrupt data is wrong for (0x%x), Expected: 0x%x, Readout: 0x%x. \n", intr_addr, intr_data, readout);
					//return;
                                        switch(readout & 0xff)
                                       {
                                        case INTR_SUS:   printf ( "  Suspend interrupt    \n");                 break;
                                        case INTR_RES:   printf ( "  Resume interrupt   \n");                   break;
                                        case INTR_RST:   printf ( "  Reset interrupt   \n");                    break;
                                        case INTR_SOF:   printf ( "  SOF interrupt   \n");                      break;
                                        case INTR_CONN:  printf ( "  Connect interrupt   \n");                  break;
                                        case INTR_DISCON:printf ( "  Disconnect interrupt   \n");       break;
                                        case INTR_SREQ:  printf ( "  Session request interrupt   \n");  break;
                                        case INTR_VBERR:
                                                     {   printf ( "  VBus error interrupt   \n");
                                                         tmp = MGC_Read8(BASE_ADDR, 0x60);
        						 tmp &= 0xfE;
       							 //printf("Write value to address 0x60 value=%02x  \n",tmp);
        						 //printf("Clear Session Bit \n");
        						 MGC_Write8(BASE_ADDR, 0x60, tmp);
                                                         udelay(5000000);
                                                         tmp = MGC_Read8(BASE_ADDR, 0x60);
                                                         tmp |= 0x01;
                                                         //printf("Write value to address 0x60 value=%02x  \n",tmp);
                                                         //printf("Set Session Bit \n");
                                                         MGC_Write8(BASE_ADDR, 0x60, tmp);
                                                         break;
                                                     }
                                        default:                 /*printf ( "  Multiple USB interrupts   \n");*/    break;
                                       }

					goto LOOP;
				}
				/*switch(readout & 0xff)
				{
					case INTR_SUS:   printf ( "  Suspend interrupt    \n"); 		break;
					case INTR_RES:   printf ( "  Resume interrupt   \n");   		break;
					case INTR_RST:   printf ( "  Reset interrupt   \n");    		break;
					case INTR_SOF:   printf ( "  SOF interrupt   \n");      		break;
					case INTR_CONN:  printf ( "  Connect interrupt   \n");  		break;
					case INTR_DISCON:printf ( "  Disconnect interrupt   \n");      	break;
					case INTR_SREQ:  printf ( "  Session request interrupt   \n");	break;
					case INTR_VBERR: printf ( "  VBus error interrupt   \n");      	break;
					default: 		 printf ( "  Multiple USB interrupts   \n");	break;
				}*/
			}

			break;
		}
		case REG_DMA_INTR:
		{
			do {
				readout = MGC_Read16(BASE_ADDR, intr_addr);
				//printf("Read interrupt for (0x%x)\n", intr_addr);
				udelay(1000000);
			}while(readout==0x0 && iloop++ < 0x5);

			if(readout==0x0) {
				printf(" ERROR : No interrupt for (0x%x) \n", intr_addr);
				return -1;
			}
			else {
				if(readout==intr_data) {
					readout=intr_data;
					//printf(" Success :(0x%x)interrupt for (0x%x) detected \n", readout,intr_addr);
				}
				else
				{
					printf(" ERROR : interrupt data is wrong for (0x%x), Expected:0x%x, Readout: 0x%x \n", intr_addr, intr_data, readout);
					return -1;
				}
				//printf ( "  DMA interrupt (%x) \n", readout & 0xffff);

				break;
			}

			break;
		}
	}

	return 0;
}

/* Check the WriteCount register and check that the FIFO contents matches sent data */
unsigned char MGC_ReadFIFO_DMA(void)
{
	int i;
	unsigned char rbytes = MGC_Read8(BASE_ADDR, 0x18);

	MGC_Write32(BASE_ADDR, 0x208, dev_descriptor); /* setting DMA addr */
	MGC_Write32(BASE_ADDR, 0x20c, rbytes); /* setting byte numbers that will be received */
	MGC_Write16(BASE_ADDR, 0x204, DMACNTRL_ENAB | DMACNTRL_INTE); /* setting CTRL regs */
{
	/* check interrupt for judging whether the DMA was finished or not */
	MGC_CheckIntr (REG_DMA_INTR, 0x01, BE_8_0);

	/* display the device descriptor */
	unsigned int endAddr = MGC_Read32(BASE_ADDR, 0x208);
	printf("\n(%08x - %08x = %d)[",  (unsigned int)endAddr,  (unsigned int)dev_descriptor,  (unsigned int)endAddr - (unsigned int)dev_descriptor);
	for (i = 0; i < rbytes; i++) printf(" %02x", dev_descriptor[i]); printf("]\n");
}

	return rbytes;
}

/* Load data into the endpoint's FIFO over the MCU interface from MCUTxdata */
void MGC_Load_DMA(void)
{
	unsigned int dma_mem_addr = (unsigned int)get_desc_cmd;

	/* setting DMA addrress */
	MGC_Write32(BASE_ADDR,0x208, dma_mem_addr);

	/* setting byte numbers that will be sent */
	MGC_Write32(BASE_ADDR,0x20c, 8);
	MGC_Write16(BASE_ADDR,0x204, DMACNTRL_ENAB | DMACNTRL_INTE | DMACNTRL_DIR);

	/* setting CTRL register */
	MGC_Write16(BASE_ADDR,0x204, DMACNTRL_ENAB | DMACNTRL_INTE | DMACNTRL_DIR);

	return;
}

/* Standard devicd requests: Get Descriptor */
void usb_get_descriptor2(void)
{
	MGC_Load_DMA();
	MGC_CheckIntr(REG_DMA_INTR, 0x01, BE_8_0);

	MGC_Write8(BASE_ADDR, REG_EP0_CSR, CSR0_SETUP | CSR0_TPR); /* send setup packet */
	//printf(" send setup packet \n");

	MGC_CheckIntr(REG_INTRTX, 0x01, BE_8_0);
	MGC_CheckReg(REG_EP0_CSR, CSR0_NULL, BE_8_0, 0);
	//printf(" setup packet has been transmitted \n");

	MGC_Write8(BASE_ADDR, REG_EP0_CSR, CSR0_REQP);
	//printf(" send a IN packet \n");

	MGC_CheckIntr(REG_INTRTX, 0x01,BE_8_0);
	MGC_CheckReg(REG_EP0_CSR, CSR0_RPR, BE_8_0, 0);
	//printf(" IN packet is sent (%04x)\n", MGC_Read16(BASE_ADDR, 0x18));

	MGC_ReadFIFO_DMA();
	MGC_Write8(BASE_ADDR, REG_EP0_CSR, CSR0_NULL);
	//printf(" read packet is OK \n");
	MGC_Write8(BASE_ADDR, REG_EP0_CSR, CSR0_TPR | CSR0_STATP);

	MGC_CheckIntr(REG_INTRTX, 0x01,BE_8_0);
	MGC_Write8(BASE_ADDR, REG_EP0_CSR, CSR0_NULL);

	//printf(" OVER \n");

	return;
}

int MGC_init2(void)
{
	int tmp = 0;
	//int addr =0;
	/*printf(" [0x62] = %02x \n", MGC_Read8(BASE_ADDR, 0x62));
	printf(" [0x63] = %02x \n", MGC_Read8(BASE_ADDR, 0x63));
	printf(" [0x64] = %02x \n", MGC_Read8(BASE_ADDR, 0x64));
	printf(" [0x65] = %02x \n", MGC_Read8(BASE_ADDR, 0x65));
	printf(" [0x66] = %02x \n", MGC_Read8(BASE_ADDR, 0x66));
	printf(" [0x67] = %02x \n", MGC_Read8(BASE_ADDR, 0x67));*/
//        MGC_Ulpi_sta_read(BASE_ADDR);
	MGC_Write8(BASE_ADDR, 0x0b, 0xff); /* enable all of intrs */
	//printf(" wait a session start \n");
	//MGC_CheckIntr(0x0a, 0x40, BE_8_0); /* wait a session start */
	tmp = MGC_Read8(BASE_ADDR, 0x60);
	tmp |= 0x01;
	//printf("Write value to address 0x60 value=%02x  \n",tmp);
	MGC_Write8(BASE_ADDR, 0x60, tmp);

	//printf(" start a session \n");
        udelay(5000);
        //printf(" wait a connection interrupt \n");
	if( MGC_CheckIntr(0x0a, 0x10, BE_8_0) == -1) /* wait a connection */
		return -1;
//	MGC_Ulpi_sta_read(BASE_ADDR);
//printf("here 622 ................\n");
	MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);
//printf("here 624 ................\n");
//        tmp=0x55;
//        addr = ULPI_SCRATCH;
//        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
   //     printf("PRESS ANY KEY TO CONTINUE,before set reset\n");
   //     getc();
//        MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN | PWR_SUSE);
//printf("REG_POWER: 0x%x\n", MGC_Read8(BASE_ADDR, REG_POWER));
        MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN);
/*{
	int i;
	for(i=0;i<5;i++){
		printf("REG_POWER: 0x%x  i= %d\n", MGC_Read8(BASE_ADDR, REG_POWER), i);
		udelay(10*1000);
	}
}*/
        //MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN);
	//the following is added by xiongdh@gmail.com
#if 0
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        printf("Read value from address REG_POWER value=%02x  \n",tmp);
        tmp &=0xDF;
        MGC_Write8(BASE_ADDR, REG_POWER,tmp);
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        printf("After clear Hs_Enab,Read value from address REG_POWER value=%02x  \n",tmp);
        MGC_Write8(BASE_ADDR, REG_POWER, tmp | PWR_RST | PWR_SUSE);
        printf("not set HS_Enab = 1 \n");

#endif
	//printf(" start a reset \n");
//        MGC_Ulpi_sta_read(BASE_ADDR);
        sleep(500);
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        //printf("After Reset,Read value from address REG_POWER value=%02x  \n",tmp);
        tmp &=0xF7;
        MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);
     //   printf("PRESS ANY KEY TO CONTINUE,before clear the reset bit\n");
    //    getc();
	MGC_Write8(BASE_ADDR, REG_POWER, tmp);
	//printf(" clear the Reset bit \n");
//        MGC_Ulpi_sta_read(BASE_ADDR);
//printf("REG_POWER: 0x%x\n", MGC_Read8(BASE_ADDR, REG_POWER));

#if 0
        printf("SET ULPI_OTG_CTL DRV_VBUS DRV_VBUS_EXT  0x03 AGAIN\n");
        addr = ULPI_OTG_CTL;
        MGC_HdrcReadUlpiReg(BASE_ADDR, addr, &tmp);
        tmp |= 0x40;
        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
        /* set the DRV_VBUS = 1, Turn on the PHY led */
        MGC_HdrcReadUlpiReg(BASE_ADDR, addr, &tmp);
        tmp |= 0x20;
        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
#endif
         //the following is added by xiongdh@gmail.com
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        //printf("After clear reset bit,Read value from address REG_POWER value=%02x  \n",tmp);

	MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);

	MGC_CheckIntr(REG_INTRUSB, INTR_SOF, BE_8_0); /* wait a SOF intr */
	//printf(" a SOF intr is OK \n");

	return 0;
}

int do_usbinit(void)
{
	//printf(" software reset USB and ULPI config\n");
	//MGC_EXTRA_INIT();
	//MGC_Ulpi_setting(BASE_ADDR);
	//MGC_Ulpi_sta_read(BASE_ADDR);
	//printf(" wait a session start \n");
	//MGC_CheckIntr(0x0a, 0x40, BE_8_0); /* wait a session start */
	//printf(" software reset USB and ULPI config\n");
	MGC_EXTRA_INIT();
	MGC_Ulpi_setting(BASE_ADDR);
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
///	MGC_Ulpi_sta_read(BASE_ADDR);
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
	if ( MGC_init2() == -1 )
		return -1;
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
      	return 0;
}

void MGC_simpletest(void)
{
	int tmp = 0;
	//int addr =0;
	/*printf(" [0x62] = %02x \n", MGC_Read8(BASE_ADDR, 0x62));
	printf(" [0x63] = %02x \n", MGC_Read8(BASE_ADDR, 0x63));
	printf(" [0x64] = %02x \n", MGC_Read8(BASE_ADDR, 0x64));
	printf(" [0x65] = %02x \n", MGC_Read8(BASE_ADDR, 0x65));
	printf(" [0x66] = %02x \n", MGC_Read8(BASE_ADDR, 0x66));
	printf(" [0x67] = %02x \n", MGC_Read8(BASE_ADDR, 0x67));*/
//        MGC_Ulpi_sta_read(BASE_ADDR);
	MGC_Write8(BASE_ADDR, 0x0b, 0xff); /* enable all of intrs */
	//printf(" wait a session start \n");
	//MGC_CheckIntr(0x0a, 0x40, BE_8_0); /* wait a session start */
	tmp = MGC_Read8(BASE_ADDR, 0x60);
	tmp |= 0x01;
	//printf("Write value to address 0x60 value=%02x  \n",tmp);
	MGC_Write8(BASE_ADDR, 0x60, tmp);

	//printf(" start a session \n");
        udelay(5000);
        //printf(" wait a connection interrupt \n");
	MGC_CheckIntr(0x0a, 0x10, BE_8_0); /* wait a connection */
//	MGC_Ulpi_sta_read(BASE_ADDR);

	MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);
//        tmp=0x55;
//        addr = ULPI_SCRATCH;
//        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
   //     printf("PRESS ANY KEY TO CONTINUE,before set reset\n");
   //     getc();
//        MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN | PWR_SUSE);
        MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN);
        //MGC_Write8(BASE_ADDR, REG_POWER, PWR_RST| PWR_HSEN);
	//the following is added by xiongdh@gmail.com
#if 0
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        printf("Read value from address REG_POWER value=%02x  \n",tmp);
        tmp &=0xDF;
        MGC_Write8(BASE_ADDR, REG_POWER,tmp);
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        printf("After clear Hs_Enab,Read value from address REG_POWER value=%02x  \n",tmp);
        MGC_Write8(BASE_ADDR, REG_POWER, tmp | PWR_RST | PWR_SUSE);
        printf("not set HS_Enab = 1 \n");

#endif
	//printf(" start a reset \n");
//        MGC_Ulpi_sta_read(BASE_ADDR);
        sleep(500);
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        //printf("After Reset,Read value from address REG_POWER value=%02x  \n",tmp);
        tmp &=0xF7;
        MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);
     //   printf("PRESS ANY KEY TO CONTINUE,before clear the reset bit\n");
    //    getc();
	MGC_Write8(BASE_ADDR, REG_POWER, tmp);
	//printf(" clear the Reset bit \n");
//        MGC_Ulpi_sta_read(BASE_ADDR);

#if 0
        printf("SET ULPI_OTG_CTL DRV_VBUS DRV_VBUS_EXT  0x03 AGAIN\n");
        addr = ULPI_OTG_CTL;
        MGC_HdrcReadUlpiReg(BASE_ADDR, addr, &tmp);
        tmp |= 0x40;
        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
        /* set the DRV_VBUS = 1, Turn on the PHY led */
        MGC_HdrcReadUlpiReg(BASE_ADDR, addr, &tmp);
        tmp |= 0x20;
        MGC_HdrcWriteUlpiReg(BASE_ADDR, addr, tmp);
#endif
         //the following is added by xiongdh@gmail.com
        tmp = MGC_Read8(BASE_ADDR, REG_POWER);
        //printf("After clear reset bit,Read value from address REG_POWER value=%02x  \n",tmp);

	MGC_CheckReg(REG_DEVCTL, DVC_FSDEV | DVC_VBUS | DVC_HMODE | DVC_SESS, BE_8_0, 0);

	MGC_CheckIntr(REG_INTRUSB, INTR_SOF, BE_8_0); /* wait a SOF intr */
	//printf(" a SOF intr is OK \n");

	usb_get_descriptor2();

        tmp = MGC_Read8(BASE_ADDR, 0x60);
        tmp &= 0xfE;
        //printf("Write value to address 0x60 value=%02x  \n",tmp);
        //printf("Clear Session Bit \n");
        MGC_Write8(BASE_ADDR, 0x60, tmp);

	return;
}

int do_usbtest_md (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf(" usbtest - test USB host controller of ORION 1.4 \n");
	//printf(" software reset USB and ULPI config\n");
	//MGC_EXTRA_INIT();
	//MGC_Ulpi_setting(BASE_ADDR);
	//MGC_Ulpi_sta_read(BASE_ADDR);
	//printf(" wait a session start \n");
	//MGC_CheckIntr(0x0a, 0x40, BE_8_0); /* wait a session start */
	//printf(" software reset USB and ULPI config\n");
	MGC_EXTRA_INIT();
	MGC_Ulpi_setting(BASE_ADDR);
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
///	MGC_Ulpi_sta_read(BASE_ADDR);
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
	MGC_simpletest();
//printf(" I'm here ...... %s, %d. \n", __FUNCTION__, __LINE__);
      	return 0;
}

U_BOOT_CMD(
	usbtest, 1, 1, do_usbtest_md,
	"usbtest - test USB host controller of ORION 1.4 \n",
	"\n"
);

#if defined(CONFIG_AUTO_UPDATE) && defined(CONFIG_USB_STORAGE)

int do_update (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	/*int i,ii;

	MGC_EXTRA_INIT();
	MGC_Write8(BASE_ADDR, 0x70, 0x01);
	udelay(50000);
	MGC_Write8(BASE_ADDR, 0x0b, 0xff); // enable all of intrs
	i = MGC_Read8(BASE_ADDR, 0x60);
	i |= 0x01;
	MGC_Write8(BASE_ADDR, 0x60, i);
    udelay(1000000);

	i = MGC_Read8(BASE_ADDR, 0x0a);
	if(i & 0x10)
		MGC_Write8(BASE_ADDR, 0x0b, 0xff);// disable all of intrs
	else{
		printf("update exit: No USB Connect!\n");
		return	0;
	}

	for (ii=0; ii<3000; ii++)
		udelay(1000);*/

	do_auto_update();
    return 0;
}

U_BOOT_CMD(
	update, 1, 1, do_update,
	"update - update your u-boot.bin / uImage / ramdiskg.img by USB storage device. \n",
	"\n"
);
#endif

















