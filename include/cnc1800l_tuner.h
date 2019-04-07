
#ifndef	_CNC1800L_TUNER_H_
#define	_CNC1800L_TUNER_H_

#include <common.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum { 
	TUNER_DVBS,
	TUNER_DVBC
} TUNER_DEV_TYPE;

typedef enum {
        TUNER_FEC_NONE = 0,
        TUNER_FEC_1_2,
        TUNER_FEC_2_3,
        TUNER_FEC_3_4,
        TUNER_FEC_4_5,
        TUNER_FEC_5_6,
        TUNER_FEC_6_7,
        TUNER_FEC_7_8,
        TUNER_FEC_8_9,
        TUNER_FEC_AUTO
} CSTUNER_FEC_RATES;

typedef enum {
	TUNER_STATUS_UNLOCKED = 0,
	TUNER_STATUS_FE_LOCKED,
	TUNER_STATUS_PLL_LOCKED,
	TUNER_STATUS_SCANNING,
	TUNER_STATUS_NOT_FOUND
} TUNER_STATUS_E;

typedef enum {
	INVERSION_NORMAL = 0,
	INVERSION_INVERTED,
	INVERSION_AUTO
} TUNER_SPECTRAL_INVERSION_E;

typedef enum {
	QAM_4 = 0,
	QAM_16,
	QAM_32,
	QAM_64,
	QAM_128,
	QAM_256,
	QAM_AUTO
} TUNER_QAM_MODULATION_E;

typedef struct {
	unsigned int frequency;		/* MHz */
	unsigned int symbol_rate;	/* MS/s */

	/* dvbc */
	TUNER_QAM_MODULATION_E modulation;
	TUNER_SPECTRAL_INVERSION_E inversion;

	/* dvbs */
	CSTUNER_FEC_RATES FECRates;
	unsigned int  LnbFrequency;     /* MHz */
	unsigned int  Polarization;     /* 'V' FALSE 'H' TRUE */

} TUNER_PARAMS_S;

int	CSTUNER_Init(void);
int	CSTUNER_Open(TUNER_DEV_TYPE type);
int	CSTUNER_Close(void);
int	CSTUNER_SetFrequency(TUNER_PARAMS_S *params);
int	CSTUNER_GetTunerInfo(TUNER_PARAMS_S *params);
int	CSTUNER_ReadStatus(TUNER_STATUS_E *status);

#ifdef __cplusplus
}
#endif

#endif	/*	_CS_TUNER_LG_H_	*/
