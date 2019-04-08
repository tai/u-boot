#include <cnc1800l_tuner.h>
#include <cnc1800l_demux.h>

#define CHECK_DEMUX_RET(x) do { \
    if ((x) != DEMUX_SUCCEED) { \
        printf("ERROR: func %s, line %d\n", __FUNCTION__, __LINE__); \
        return -1;  \
    }   \
}while(0)

static unsigned char section_data[4096];

int test_xport()
{
    int i;
    TUNER_PARAMS_S tuner_param;
    TUNER_STATUS_E tuner_status;
    int n = 25;
    CSDEMUX_HANDLE chl_handle;
    CSDEMUX_HANDLE pidft_handle;
    CSDEMUX_HANDLE filter_handle;
    unsigned char filter[12];
    unsigned char mask[12];
    int data_size = 0;

    CSTUNER_Open(TUNER_DVBC);

    if (0 > CSTUNER_Init())
    {
        printf("CSTUNER_Init failed.\n");
        return -1;
    }

    tuner_param.frequency = 642;
    tuner_param.symbol_rate = 6875;
    tuner_param.modulation = QAM_64;
    tuner_param.inversion = INVERSION_AUTO;

    if (0 > CSTUNER_SetFrequency(&tuner_param))
    {
        printf("CSTUNER_SetFrequency failed.\n");
        return -1;
    }

    while(n--)
    {
        udelay(200000);
        if (0 > CSTUNER_ReadStatus(&tuner_status))
        {
            printf("CSTUNER_ReadStatus failed.\n");
            return -1;
        }

        if (tuner_status == TUNER_STATUS_FE_LOCKED)
        {
            printf("tuner locked.\n");
            break;
        }
    }

    CHECK_DEMUX_RET( CSDEMUX_Init() );
    chl_handle = CSDEMUX_CHL_Open(0);
    if (chl_handle == NULL)
    {
        printf("CSDEMUX_CHL_Open failed.\n");
        return -1;
    }
    CHECK_DEMUX_RET( CSDEMUX_CHL_SetInputMode(chl_handle, DEMUX_INPUT_MOD_TUNER) );
    CHECK_DEMUX_RET( CSDEMUX_CHL_SetTunerInputMode(chl_handle, DEMUX_TUNER_MOD_PARALLEL) );
    //CHECK_DEMUX_RET( CSDEMUX_CHL_SetTunerInputMode(chl_handle, DEMUX_TUNER_MOD_SERIAL) );
    CHECK_DEMUX_RET( CSDEMUX_CHL_Enable(chl_handle) );
    pidft_handle = CSDEMUX_PIDFT_Open(0);
    if (pidft_handle == NULL)
    {
        printf("CSDEMUX_PIDFT_Open failed.\n");
        return -1;
    }
    CHECK_DEMUX_RET( CSDEMUX_PIDFT_SetChannel(pidft_handle, 0) );
    CHECK_DEMUX_RET( CSDEMUX_PIDFT_SetPID(pidft_handle, 0) );
    filter_handle = CSDEMUX_Filter_Open(0);
    if (filter_handle == NULL)
    {
        printf("CSDEMUX_Filter_Open failed.\n");
        return -1;
    }
    CHECK_DEMUX_RET( CSDEMUX_Filter_SetFilterType(filter_handle, DEMUX_FILTER_TYPE_SEC) );
    memset(filter, 0, 12);
    memset(mask, 0, 12);
    mask[0] = 0xff;
    CHECK_DEMUX_RET( CSDEMUX_Filter_SetFilter(filter_handle, filter, mask) );
    CHECK_DEMUX_RET( CSDEMUX_Filter_AddPID(filter_handle, 0) );

    CHECK_DEMUX_RET( CSDEMUX_PIDFT_Enable(pidft_handle) );
    CHECK_DEMUX_RET( CSDEMUX_Filter_Enable(filter_handle) );

    while(1)
    {
        udelay(200000);
        CHECK_DEMUX_RET( CSDEMUX_Filter_CheckDataSize(filter_handle, &data_size) );
        if (data_size != 0)
        {
            CHECK_DEMUX_RET( CSDEMUX_Filter_ReadSectionData(filter_handle, section_data, &data_size) );
            printf("got a pat section, size is %d\n", data_size);
            for (i=0; i<data_size; i++)
            {
                printf("0x%02x,", section_data[i]);
                if (i%10 == 9)
                    printf("\n");
            }
            break;
        }
    }

    return 0;
}

