#include <cnc1800l_tuner.h>
//#include "tda10023.h"
//#include "stv0288.h"
#include "tuner_i2c.h"
#include "mt_fe_def.h"


/* Global variables */
static int tuner_use_count;
static TUNER_DEV_TYPE dev_type;
static TUNER_PARAMS_S tuner_info;
static void _tuner_hw_reset(void)
{
	printf("Reset tuner!\n");
	gpio_hw_set_direct(6, 1);
	gpio_hw_write(6, 0);
	udelay(400000);
	gpio_hw_write(6, 1);
	udelay(400000);
}

/*
 * 0 -V, 1 - H
 */
static void _tuner_set_polarization(unsigned int polarization)
{
	printf("Set dvbs Polarization!\n");
	gpio_hw_set_direct(12, 1);
	gpio_hw_write(12, 1);	/* LNB Power On/Off */
	gpio_hw_set_direct(13, 1);
	gpio_hw_write(13, polarization != 0 ? 1 : 0);
}

/*
 * the implementation of DiSEqC.
 */

struct diseqc_master_cmd {
	unsigned char msg[6];
	unsigned char msg_len;
};

static int _stv0288_set_FEC(unsigned char fec)
{
	switch (fec) {
	case TUNER_FEC_AUTO:
		stv0288_write(0x31, 0x1f);
		break;
	case TUNER_FEC_1_2:
		stv0288_write(0x31, 0x01);
		break;
	case TUNER_FEC_2_3:
		stv0288_write(0x31, 0x02);
		break;
	case TUNER_FEC_3_4:
		stv0288_write(0x31, 0x04);
		break;
	case TUNER_FEC_5_6:
		stv0288_write(0x31, 0x08);
		break;
	case TUNER_FEC_7_8:
		stv0288_write(0x31, 0x10);
		break;
	default:
		break;
	}
	return 0;
}

static int stv0288_wait_diseqc_fifo(int timeout)
{
#if 0
	int ii = 0;

	while (stv0288_read(0x0a) & 1)
	{
		if (++ii > timeout) return -1;

		udelay(10000);
	};
#endif

	return 0;
}

static int stv0288_wait_diseqc_idle(int timeout)
{

#if 0
	int ii = 0;

	while ((stv0288_read(0x0a) & 3) != 2) {
		if (++ii > timeout) return -1;

		udelay(10000);
	};
#endif
	return 0;
}

static int stv0288_send_diseqc_msg(struct diseqc_master_cmd *m)
{

#if 0
	int i;
	unsigned char val;

	if (stv0288_wait_diseqc_idle(100) < 0)
		return -1;

	val = stv0288_read(0x08);

	stv0288_write(0x08, (val & ~0x7) | 0x6); /* DiSEqC mode */

	for (i = 0; i < m->msg_len; i++) {
		if (stv0288_wait_diseqc_fifo(100) < 0)
			return -3;

		stv0288_write(0x09, m->msg[i]);
	}

	if (stv0288_wait_diseqc_idle(100) < 0)
		return -5;
#endif

	return 0;
}

int CSTUNER_Open(TUNER_DEV_TYPE type)
{
	tuner_use_count++;
	dev_type = type;
	/* TODO : do first init */
	if (tuner_use_count < 2) {
		;
	}
	return 0;
}
int CSTUNER_Init(void)
{
	printf("CSTUNER_Init function called. \n");
	tuner_i2c_init();
	_tuner_hw_reset();
	switch (dev_type) {
	case TUNER_DVBS:
		printf("Init DVBS_stv0288\n");
		//stv0288_init();
		//mt_fe_dmd_ds3k_init();
		break;
	case TUNER_DVBC:
		printf("Init M88DC2800!\n");
		//M88DC2000TunerSet(params->frequency,params->qam_params.symbol_rate,params->qam_params.modulation,1,28800);
		//p10023_demodu_init();
		break;
	default:
		printf("No such device type!!!\n");
	}
	return 0;
}
int CSTUNER_Close(void)
{
	printf("CSTUNER_Close function called. \n");
	if (tuner_use_count-- > 0)
		return 0;

	/* TODO : do some clean jobs */
	return 0;
}

int CSTUNER_SetFrequency(TUNER_PARAMS_S * params)
{
	printf("CSTUNER_SetFrequency function called. \n");
	if (!params) {
		printf("Error tuner params!\n");
		return -1;
	}
	switch (dev_type) {
	case TUNER_DVBS:
		printf("\nSet tuner param : \n"
		       "Frequency:%d (M)  Symbol Rate:%d (K)\n"
		       "LnbFrequency:%d (M)\n"
		       "Polarization:%d (0 -V, 1 - H)\n", params->frequency,
		       params->symbol_rate, params->LnbFrequency,
		       params->Polarization);
		tuner_info.frequency =
			(params->LnbFrequency > params->frequency) ?
			(params->LnbFrequency - params->frequency) :
			(params->frequency - params->LnbFrequency);
		tuner_info.symbol_rate = params->symbol_rate;
		tuner_info.LnbFrequency = params->LnbFrequency;
		tuner_info.Polarization = params->Polarization;
		//_tuner_set_polarization(tuner_info.Polarization);
		//_stv0288_set_FEC(tuner_info.FECRates);
		//VZ0295SetFrequency(tuner_info.frequency,
				 //  tuner_info.symbol_rate);

		//mt_fe_dmd_ds3k_connect(params->frequency, params->symbol_rate, MtFeType_DvbS_Unknown);
		break;
	case TUNER_DVBC:
		printf("\nSet tuner param : \n"
		       "Frequency:%d (M)  Symbol Rate:%d (K)\n"
		       "Qam:%d (1-16,2-32,3-64,4-128,5-256,6-AUTO,)\n"
		       "Inversion:%d (0-NORMAL,1-INVERTED,2-AUTO)\n",
		       params->frequency, params->symbol_rate,
		       params->modulation, params->inversion);
		tuner_info.frequency = params->frequency;
		tuner_info.symbol_rate = params->symbol_rate;
		tuner_info.modulation = params->modulation;
		tuner_info.inversion = params->inversion;
		//p10023_demodu(tuner_info.frequency, tuner_info.symbol_rate,
			   //   tuner_info.modulation, tuner_info.inversion);

		M88DC2000TunerSet(params->frequency*1000,params->symbol_rate,params->modulation,params->inversion,28800);
		break;
	default:
		printf("No such device type!!!\n");
	}
	return 0;
}

int CSTUNER_GetTunerInfo(TUNER_PARAMS_S * params)
{
	switch (dev_type) {
	case TUNER_DVBS:
		params->frequency = tuner_info.frequency + tuner_info.symbol_rate;
		params->symbol_rate = tuner_info.symbol_rate;
		params->LnbFrequency = tuner_info.LnbFrequency;
		params->Polarization = tuner_info.Polarization;
		break;
	case TUNER_DVBC:
		params->frequency = tuner_info.frequency;
		params->symbol_rate = tuner_info.symbol_rate;
		params->modulation = tuner_info.modulation;
		params->inversion = tuner_info.inversion;
		break;
	default:
		printf("No such device type!!!\n");
	}
	return 0;
}

int CSTUNER_ReadStatus(TUNER_STATUS_E * status)
{
	int st = 0;
	switch (dev_type) {
	case TUNER_DVBS:
		printf("TUNER_DVBS:   ");
		//st = check_lock_status();
		//mt_fe_dmd_ds3k_get_lock_state(&st);

		printf("sssssssssssssssssssssssstttttttttttttttttttttttttt=%d\n",st);
		if(st==2)
		{
			st=1;
		}else if(st==1)
		{
			st=0;
		}
		break;
	case TUNER_DVBC:
		printf("TUNER_DVBC:   ");
		//st = p10023_tuner_lock_check();
		st=M88DC2000GetLock();
		break;
	default:
		printf("No such device type!!!\n");
	}
	if (st == 1) {
		printf("*********** TUNER LOCKED **********\n");
		*status = TUNER_STATUS_FE_LOCKED;
	} else {
		printf("*********** TUNER NOT LOCK ********\n");
		*status = TUNER_STATUS_UNLOCKED;
	}
	return 0;
}

int CSTUNER_SetDisecQ(unsigned char feed_number)
{
	struct diseqc_master_cmd cmds;

	cmds.msg[0] = 0xe0; /* send commands to device, response is not needed.*/
	cmds.msg[1] = 0x00; /* to all of devices. */
	cmds.msg[2] = 0x6b; /* go to a dedicated position. */
	cmds.msg[3] = feed_number; /* a specified postion where will go to. */
	cmds.msg_len = 4;

	stv0288_send_diseqc_msg(&cmds);

	return 0;
}
