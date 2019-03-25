#include <common.h>
#include <asm/io.h>
#include "cnc1800l_i2c.h"

#if defined(CONFIG_HARD_I2C)

#define i2c_writeb(v,a)    writeb(v, (CONFIG_SYS_I2C_BASE + (a)))
#define i2c_writew(v,a)    writew(v, (CONFIG_SYS_I2C_BASE + (a)))
#define i2c_writel(v,a)    writel(v, (CONFIG_SYS_I2C_BASE + (a)))

#define i2c_readb(a)       readb((CONFIG_SYS_I2C_BASE + (a)))
#define i2c_readw(a)       readw((CONFIG_SYS_I2C_BASE + (a)))
#define i2c_readl(a)       readl((CONFIG_SYS_I2C_BASE + (a)))

#define I2C_CLK_MHZ  (CONFIG_SYS_I2C_CLK/1000000)

#define I2C_DEBUG_LEVEL0	(0)	/* Quiet   */
#define I2C_DEBUG_LEVEL1	(1)	/* Audible */
#define I2C_DEBUG_LEVEL2	(2)	/* Loud    */
#define I2C_DEBUG_LEVEL3	(3)	/* Noisy   */

//#define CONFIG_CNC1800L_I2C_DEBUG
//#define CONFIG_I2C_DEBUG_VERBOSE 3

#ifdef CONFIG_CNC1800L_I2C_DEBUG
#define DEBUG(n, args...)				\
 	do {						\
		if (n <= CONFIG_I2C_DEBUG_VERBOSE)	\
			printf(args);			\
	} while(0)
#else /* CONFIG_I2C_DEBUG */
#define DEBUG(n, args...) do { } while(0)
#endif

static unsigned int previous_speed_cnt;


static unsigned int ss_ic_sclh_cnt;
static unsigned int ss_ic_scl_cnt;
static unsigned int fs_ic_sclh_cnt;
static unsigned int fs_ic_scl_cnt;
static unsigned int default_speed_cnt;

static uchar previous_chip = 0x7f;
int i2c_probe (uchar chip)
{
    int i = 0;
    int time_cnt;
    int fifocount;
    i2c_writew(0x0,IC_RESET);
    i=i2c_readw(IC_RAW_INTR_STAT);
    fifocount = i2c_readw(IC_TXFLR);
    i2c_writew(0x1, IC_ENABLE);
    i2c_writew(((chip<<1)&0xff)|0x100,IC_DATA_CMD);
    i2c_writew(0x00,IC_DATA_CMD);
    i2c_writew(0x00,IC_DATA_CMD);
    i2c_writew(0x00,IC_DATA_CMD);
    i2c_writew(0x00,IC_DATA_CMD);
    i2c_writew(0x01,IC_TX_TL);
//     if (i2c_readw(IC_STAT) & 0x8) {
//         i = i2c_readw(IC_DATA_CMD);
//     }
    time_cnt =0;
    do {
        time_cnt ++;
        i=i2c_readw(IC_RAW_INTR_STAT);
    } while (i==0 && time_cnt < 1000000);
    if (time_cnt>=1000000) {              //Bus Err
        i2c_writew(0x03,IC_FIFO_CLR);	  //clear tx/rx fifo
        i2c_writew(0x1,IC_RESET);
        i2c_writew(0x0,IC_RESET);
        i2c_writew(0x1,IC_ENABLE);
        i2c_writew(0x700,IC_DATA_CMD);    //write stop bit
        DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
        return 1;
    }
    if (i&0x20) {								//No ACK
        i2c_writew(0x20,IC_INTR_CLR);
        i2c_writew(0x03,IC_FIFO_CLR);			//clear tx/rx fifo
        i2c_writew(0x700,IC_DATA_CMD);
        DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
        return 1;
    }
    i2c_writew(0x20,IC_INTR_CLR);
    i2c_writew(0x03,IC_FIFO_CLR);			//clear tx/rx fifo
    i2c_writew(0x700,IC_DATA_CMD);
    return 0;
}


int i2c_read(uchar chip,uint addr, int alen, uchar * buffer, int len)
{
    int i;
    int cmd_cnt;
    int data_cnt;
    unsigned int time_cnt;
    unsigned char *byte_addr = (unsigned char *) &addr;
    unsigned char *p = buffer;
    //    printf("alen =%d\n",alen);
    time_cnt=0;
    while (i2c_readw(IC_STAT)&0x20) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
    }
    i2c_writew(0x1, IC_ENABLE);
    if (alen > 0) {
        i2c_writew(((chip<<1)&0xff)|0x100,IC_DATA_CMD);
    }
    for (i = alen - 1; i >= 0; i--) {
        i2c_writew(0x200|(byte_addr[i]& 0xff),IC_DATA_CMD);
        DEBUG(I2C_DEBUG_LEVEL3,"Chipaddress=0x%x, subaddr=0x%x, written_addr=0x%x\n",
              chip, addr, byte_addr[i]);
    }
    i2c_writew(0x101|((chip<<1)&0xff),IC_DATA_CMD);
    data_cnt = 0;
    cmd_cnt = 0;
    time_cnt=0;
    while (data_cnt < len) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
        while ((cmd_cnt < len) && (i2c_readw(IC_STAT) & 0x2)) {
            if (cmd_cnt+1==len) {
                i2c_writew(0x600,IC_DATA_CMD);		//write stop bit
            } else {
                i2c_writew(0x400, IC_DATA_CMD);
            }
            cmd_cnt++;
        }
        if (i2c_readw(IC_STAT) & 0x8) {
            *p = i2c_readw(IC_DATA_CMD);
            DEBUG(I2C_DEBUG_LEVEL3,"Chipaddress=0x%x, subaddr=0x%x, readed=0x%x\n",
                  chip, addr, *p);
            p++;
            data_cnt++;
        }
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);			//clear tx/rx fifo
            if (i&0x40) {							//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {							//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
    }
    time_cnt=0;
    while (!(i2c_readw(IC_STAT)&0x04)) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);			//clear tx/rx fifo
            if (i&0x40) {							//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {							//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
    }             //wait all data send
    time_cnt=0;
    while (i2c_readw(IC_STAT) & 0x20) {
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);			//clear tx/rx fifo
            if (i&0x40) {							//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {							//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
    }
    //wait for stop signal
    return 0;
}


int i2c_write(uchar chip,uint addr, int alen,uchar * buffer, int len)
{
    int i;
    int data_cnt;
    unsigned int  time_cnt;
    unsigned char *byte_addr = (unsigned char *) &addr;
    unsigned char *p = buffer;
    //    i2c_writew(0x0,IC_RESET);
    time_cnt=0;
    while (i2c_readw(IC_STAT)&0x20) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
    }
    i2c_writew(0x1, IC_ENABLE);
    i2c_writew(((chip<<1)&0xff)|0x100,IC_DATA_CMD);
    for (i = alen - 1; i >= 0; i--) {
        i2c_writew(0x200|byte_addr[i],IC_DATA_CMD);
    }
    data_cnt = 0;
    time_cnt=0;
    while (data_cnt < len) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
        if (i2c_readw(IC_STAT) & 0x2) {
            if (data_cnt!=len-1) {
                i2c_writew(0x200|(*p), IC_DATA_CMD);
            } else {
                i2c_writew(0x300|(*p),IC_DATA_CMD);				//Stop
            }
            p++;
            data_cnt++;
        }
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);				//clear tx/rx fifo
            if (i&0x40) {								//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {								//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);		//Stop
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
    }
    time_cnt=0;
    while (!(i2c_readw(IC_STAT)&0x04)) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);				//clear tx/rx fifo
            if (i&0x40) {						//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {						//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);				//Stop
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
    }             //wait all data send
    time_cnt=0;
    while (i2c_readw(IC_STAT) & 0x20) {
        if (time_cnt>=100000) {       //Bus Err
            i2c_writew(0x1,IC_RESET);
            i2c_writew(0x0,IC_RESET);
            i2c_writew(0x1,IC_ENABLE);
            i2c_writew(0x700,IC_DATA_CMD);     //write stop bit
            previous_chip = 0;
            DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            return 1;
        }
        time_cnt++;
        if ((i=i2c_readw(IC_RAW_INTR_STAT)) & 0xe0) {
            DEBUG(I2C_DEBUG_LEVEL0,"Chipaddress=0x%x, subaddr=0x%x, IC_RAW_INTR_STAT=0x%x\n",
                  chip, addr, i);
            i2c_writew(0x03,IC_FIFO_CLR);				//clear tx/rx fifo
            if (i&0x40) {						//Arbitration Lost
                i2c_writew(0x40,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Arbitration lost!\n");
            }
            if (i&0x80) {
                i2c_writew(0x80,IC_INTR_CLR);
                DEBUG(I2C_DEBUG_LEVEL0,"Error:Timeout!\n");
            }
            if (i&0x20) {								//No ACK
                i2c_writew(0x20,IC_INTR_CLR);
                i2c_writew(0x700,IC_DATA_CMD);		//Stop
                DEBUG(I2C_DEBUG_LEVEL0,"Error:No Slave sends ack!\n");
            }
            previous_chip = 0;
            return -1;
        }
    }
    return 0;
}

uchar i2c_reg_read (uchar chip, uchar reg)
{
    uchar readed;
    if (i2c_read(chip, reg, 1, &readed, 1) == 1 ) {
        printf("Read I2C Error !\n");
        return 0;
    }
    return readed;
}

void  i2c_reg_write(uchar chip, uchar reg, uchar val)
{
    if (i2c_write(chip, reg, 1,&val, 1) == 1) {
        printf("Write I2C Error !\n");
    }
}

void i2c_init(int speed, int slaveaddr)
{
    //printf("Init CNC18xx I2C ");
    ss_ic_sclh_cnt = (CONFIG_SYS_I2C_CLK ) /(4*100000);
    ss_ic_scl_cnt = (CONFIG_SYS_I2C_CLK ) /(2*100000);
    fs_ic_sclh_cnt = (CONFIG_SYS_I2C_CLK) / (24 * 100000);
    fs_ic_scl_cnt = (CONFIG_SYS_I2C_CLK) / (8 * 100000);
    previous_chip = slaveaddr;
    i2c_writew(0x0, IC_ENABLE);
    if (speed > 150000) {
        i2c_writew(fs_ic_sclh_cnt, IC_SCLH_CNT);
        i2c_writew(fs_ic_scl_cnt - fs_ic_sclh_cnt, IC_SCLL_CNT);
        previous_speed_cnt = fs_ic_scl_cnt;
        default_speed_cnt = fs_ic_scl_cnt;
    } else {
        i2c_writew(ss_ic_sclh_cnt, IC_SCLH_CNT);
        i2c_writew(ss_ic_scl_cnt - ss_ic_sclh_cnt, IC_SCLL_CNT);
        previous_speed_cnt = ss_ic_scl_cnt;
        default_speed_cnt = ss_ic_scl_cnt;
    }
}

int i2c_set_bus_speed(unsigned int speed) /* only support 100KHz and 400KHz */
{
    static unsigned int speed_cnt;
    if (speed > 150000)
        speed_cnt = fs_ic_scl_cnt;
    else
        speed_cnt = ss_ic_scl_cnt;
    if (speed_cnt!=previous_speed_cnt) {
        i2c_writew(0x0, IC_ENABLE);
        while (!(i2c_readw(IC_STAT)& 0x1));					//wait disabled
        if ((speed_cnt==ss_ic_scl_cnt)) {					//Standard Speed
            i2c_writew(ss_ic_sclh_cnt,IC_SCLH_CNT);
            i2c_writew(ss_ic_scl_cnt-ss_ic_sclh_cnt,IC_SCLL_CNT);
            previous_speed_cnt = ss_ic_scl_cnt;
        } else if (speed_cnt==fs_ic_scl_cnt) {					//Fast Speed
            i2c_writew(fs_ic_sclh_cnt,IC_SCLH_CNT);
            i2c_writew(fs_ic_scl_cnt-fs_ic_sclh_cnt,IC_SCLL_CNT);
            previous_speed_cnt =fs_ic_scl_cnt;
        }
    }
    return 0;
}

unsigned int i2c_get_bus_speed(void)
{
    if (previous_speed_cnt ==  fs_ic_scl_cnt)
        return 400000;
    if (previous_speed_cnt ==  ss_ic_scl_cnt)
        return 100000;
    else
        return 0;
}

#endif
