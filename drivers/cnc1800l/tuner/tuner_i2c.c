#include "tuner_i2c.h"

int tuner_i2c_init(void)
{
	printf("%s Init. \n", __FUNCTION__);
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	return 0;
}

/*This read time mode*/
/* Start | ACK | SubAddr | Ack | Data[0] |Ack .... Data[N] |Ack | Stop*/
int tuner_i2c_read(unsigned int address, int subaddr, int alen,
		unsigned char *buffer, unsigned int num)
{
	int ecode = 0;
	ecode = i2c_read(address & 0xff, subaddr & 0xff, alen, buffer, num);
	if (ecode != 0)
		printf("%s: i2c_read error! at line: %d\n", __FUNCTION__,
				__LINE__);
	udelay(400);
	return ecode;
}
int tuner_i2c_write(unsigned int address, int subaddr, int alen,
		unsigned char *buffer, unsigned int num)
{
	int ecode = 0;

#if 0
	/* Test */
	printf("write: , addr: %x subaddr %x buffer: %x ", address, subaddr,
			*buffer);

#endif /*  */
	ecode = i2c_write(address & 0xff, subaddr & 0xff, alen, buffer, num);
	if (ecode != 0)
		printf("%s: i2c_write error! at line: %d\n", __FUNCTION__,
				__LINE__);
	udelay(800);

#if 0
	/* Test */
	int i;
	unsigned char cc[num];
	tuner_i2c_read(address, subaddr, alen, cc, num);
	for (i = 0; i < num; i++)
		printf(" read %x ", cc[i]);
	printf("\n");

#endif /*  */
	return ecode;
}
int tuner_write_mask(unsigned int address, int subaddr, int alen, int mask,
		unsigned char data)
{
	int ret = 0;
	unsigned char val = 0;
	unsigned char tmp_val = 0;
	if ((mask & 0xff) == 0xff) {
		val = data;
		ret = tuner_i2c_write(address, subaddr, alen, &val, 1);
		if (ret < 0)
			printf("Error: mask.tuner_i2c_write\n");
		return ret;
	}
	tuner_i2c_read(address, subaddr, alen, &val, 1);
	tmp_val = val;
	val = (val & (~mask));
	val = (val | (data & mask));
	ret = tuner_i2c_write(address, subaddr, alen, &val, 1);
	if (ret < 0)
		printf("Error: tuner_i2c_write.\n");
	return ret;
}
