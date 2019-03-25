/*
 * XPORT interface functions
 *
 * Copyright (c) 2007 Celestial Semiconductor
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Ma Jia  jia.ma@celestialsemi.com, Celestial Semiconductor
 *
 * Copyright (c) 2008 Celestial Semiconductor
 *
 */

#include <exports.h>
#include <cnc1800l_demux.h>
#include "cnc1800l_xport.h"

/* Internal object and handle define */
#define CHECK_HANDLE_VALID(o, m) do { \
	if ((0 == o) || (o->obj_type != m)) { \
		return DEMUX_FAILED; \
	} \
} while(0)

typedef enum
{
	CSDEMUX_OBJ_CHL = 10,
	CSDEMUX_OBJ_PIDFT,
	CSDEMUX_OBJ_FILTER
} CSDEMUX_OBJ_TYPE;

typedef struct tagCSDEMUX_CHL
{
	CSDEMUX_OBJ_TYPE	obj_type;
	CSDEMUX_CHL_ID		chl_id;
	CSDEMUX_INPUT_MOD	input_mode;
	CSDEMUX_STATUS		chl_en;
	int			errno;
} CSDEMUX_CHL;

typedef struct tagCSDEMUX_PIDFT
{
	CSDEMUX_OBJ_TYPE	obj_type;
	CSDEMUX_PIDFT_ID	pid_filter_id;
	CSDEMUX_STATUS		pidft_en;
	int			errno;
} CSDEMUX_PIDFT;

typedef struct tagCSDEMUX_FILTER
{
	CSDEMUX_OBJ_TYPE	obj_type;
	CSDEMUX_FILTER_TYPE	filter_type;
	CSDEMUX_FILTER_ID	filter_id;
	CSDEMUX_STATUS		filter_en;
	int			errno;
} CSDEMUX_FILTER;

static CSDEMUX_CHL xport_chl_obj[DEMUX_CHL_NUM];	/* input channel objects */
static CSDEMUX_PIDFT xport_pidft_obj[DEMUX_PIDFT_NUM];	/* PID filter objects */
static CSDEMUX_FILTER xport_filter_obj[DEMUX_FILTER_NUM];	/* section filter objects */


CSDEMUX_RESULT CSDEMUX_Init(void)
{
	int i = 0;

	printf("u-boot xport driver init!\n");

	for(i = 0; i < DEMUX_CHL_NUM; i++){
		xport_chl_obj[i].obj_type = CSDEMUX_OBJ_CHL;
		xport_chl_obj[i].chl_id = i;
		xport_chl_obj[i].input_mode = -1;
		xport_chl_obj[i].chl_en = DEMUX_DISABLE;
		xport_chl_obj[i].errno = -1;
	}

	for(i = 0; i < DEMUX_PIDFT_NUM; i++){
		xport_pidft_obj[i].obj_type = CSDEMUX_OBJ_PIDFT;
		xport_pidft_obj[i].pid_filter_id = i;
		xport_pidft_obj[i].pidft_en = DEMUX_DISABLE;
		xport_pidft_obj[i].errno = -1;
	}

	for(i = 0; i < DEMUX_FILTER_NUM; i++ ){
		xport_filter_obj[i].obj_type = CSDEMUX_OBJ_FILTER;
		xport_filter_obj[i].filter_type = DEMUX_FILTER_UNDEF;
		xport_filter_obj[i].filter_id = i;
		xport_filter_obj[i].filter_en = DEMUX_DISABLE;
		xport_filter_obj[i].errno = -1;
	}

    if (0 > xport_init())
        return DEMUX_FAILED;

	return DEMUX_SUCCEED;
}


CSDEMUX_HANDLE CSDEMUX_CHL_Open(CSDEMUX_CHL_ID chl_id)
{
	if((chl_id < DEMUX_CHL_ID0)||(chl_id >= DEMUX_CHL_NUM)){
		printf("invalid parameters ...... %s, %d. \n", __FUNCTION__, __LINE__);
		return 0;
	}

	if(DEMUX_ENABLE == xport_chl_obj[chl_id].chl_en){
		printf("channel%d was already opened ...... %s, %d. \n",chl_id,__FUNCTION__,__LINE__);
		return 0;
	}

	xport_chl_obj[chl_id].chl_en = DEMUX_ENABLE;
	xport_chl_obj[chl_id].errno = 0;

	return &(xport_chl_obj[chl_id]);
}

CSDEMUX_RESULT CSDEMUX_CHL_Close(CSDEMUX_HANDLE handle)
{
	CSDEMUX_CHL *chl_ptr = handle;

	CHECK_HANDLE_VALID(chl_ptr, CSDEMUX_OBJ_CHL);

	if(DEMUX_ENABLE == chl_ptr->chl_en){
		if(xport_set_channel_status(chl_ptr->chl_id,DEMUX_DISABLE)){
			printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
			return DEMUX_FAILED;
		}
	}

	chl_ptr->chl_en = DEMUX_DISABLE;
	chl_ptr->input_mode = -1;

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_CHL_SetInputMode(CSDEMUX_HANDLE handle, CSDEMUX_INPUT_MOD input_mod)
{
	CSDEMUX_CHL *chl_ptr = handle;

	CHECK_HANDLE_VALID(chl_ptr, CSDEMUX_OBJ_CHL);

	if(xport_set_channel_inputmode(chl_ptr->chl_id,input_mod)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	chl_ptr->input_mode = input_mod;

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_CHL_SetTunerInputMode(CSDEMUX_HANDLE handle, CSDEMUX_TUNER_MOD tuner_mode)
{
	CSDEMUX_CHL *chl_ptr = handle;

	CHECK_HANDLE_VALID(chl_ptr, CSDEMUX_OBJ_CHL);

	if(xport_set_tuner_input_mode(chl_ptr->chl_id, tuner_mode)) {
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}


CSDEMUX_RESULT CSDEMUX_CHL_Enable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_CHL *chl_ptr = handle;

	CHECK_HANDLE_VALID(chl_ptr, CSDEMUX_OBJ_CHL);

	if(chl_ptr->input_mode == -1){
		printf("channel%d has not setting INPUTMODE, %s, %d. \n",chl_ptr->chl_id,__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}
	else{
		if(xport_set_channel_status(chl_ptr->chl_id,DEMUX_ENABLE)){
			printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		}
	}

	return DEMUX_SUCCEED;

}

CSDEMUX_RESULT CSDEMUX_CHL_Disable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_CHL *chl_ptr = handle;

	CHECK_HANDLE_VALID(chl_ptr, CSDEMUX_OBJ_CHL);

	if(xport_set_channel_status(chl_ptr->chl_id,DEMUX_DISABLE)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_HANDLE CSDEMUX_PIDFT_Open(CSDEMUX_PIDFT_ID pidfilter_id)
{
	if((pidfilter_id < DEMUX_PIDFT_ID0)||(pidfilter_id >= DEMUX_PIDFT_NUM)){
		printf("invalid parameters ...... %s, %d. \n", __FUNCTION__, __LINE__);
		return 0;
	}

	if(DEMUX_ENABLE == xport_pidft_obj[pidfilter_id].pidft_en){
		printf("pidfilter%d was already opened ...... %s, %d. \n",pidfilter_id,__FUNCTION__,__LINE__);
		return 0;
	}

	xport_pidft_obj[pidfilter_id].pidft_en = DEMUX_ENABLE;
	xport_pidft_obj[pidfilter_id].errno = 0;

	return &(xport_pidft_obj[pidfilter_id]);
}

CSDEMUX_RESULT CSDEMUX_PIDFT_Close(CSDEMUX_HANDLE handle)
{
	CSDEMUX_PIDFT *pidft_ptr = handle;

	CHECK_HANDLE_VALID(pidft_ptr, CSDEMUX_OBJ_PIDFT);

	if(DEMUX_ENABLE == pidft_ptr->pidft_en){
		if(xport_pidft_set_status(pidft_ptr->pid_filter_id,DEMUX_DISABLE)){
			printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
			return DEMUX_FAILED;
		}
	}

	pidft_ptr->pidft_en = DEMUX_DISABLE;
	pidft_ptr->errno = -1;

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_PIDFT_SetChannel(CSDEMUX_HANDLE handle, CSDEMUX_CHL_ID chl_id)
{
	CSDEMUX_PIDFT *pidft_ptr = handle;

	CHECK_HANDLE_VALID(pidft_ptr, CSDEMUX_OBJ_PIDFT);

	if((chl_id < DEMUX_CHL_ID0)||(chl_id >= DEMUX_CHL_NUM)){
		printf("invalid parameters ...... %s, %d. \n", __FUNCTION__, __LINE__);
		return DEMUX_FAILED;
	}

	if(xport_pidft_bind_chl(chl_id, pidft_ptr->pid_filter_id)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_PIDFT_SetPID(CSDEMUX_HANDLE handle, unsigned short pid)
{
	CSDEMUX_PIDFT *pidft_ptr = handle;

	CHECK_HANDLE_VALID(pidft_ptr, CSDEMUX_OBJ_PIDFT);

	if(xport_pidft_set_pid(pidft_ptr->pid_filter_id,pid)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_PIDFT_Enable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_PIDFT *pidft_ptr = handle;

	CHECK_HANDLE_VALID(pidft_ptr, CSDEMUX_OBJ_PIDFT);

	if(xport_pidft_set_status(pidft_ptr->pid_filter_id,DEMUX_ENABLE)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_PIDFT_Disable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_PIDFT *pidft_ptr = handle;

	CHECK_HANDLE_VALID(pidft_ptr, CSDEMUX_OBJ_PIDFT);

	if(xport_pidft_set_status(pidft_ptr->pid_filter_id,DEMUX_DISABLE)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_HANDLE CSDEMUX_Filter_Open(CSDEMUX_FILTER_ID filter_id)
{
	if((filter_id < DEMUX_FILTER_ID0)||(filter_id >= DEMUX_FILTER_NUM)){
		printf("invalid parameters ...... %s, %d. \n", __FUNCTION__, __LINE__);
		return 0;
	}

	if(DEMUX_ENABLE == xport_filter_obj[filter_id].filter_en){
		printf("filter%d was already opened ...... %s, %d. \n",filter_id,__FUNCTION__,__LINE__);
		return 0;
	}

	xport_filter_clear_buffer(filter_id);

	xport_filter_obj[filter_id].filter_en = DEMUX_ENABLE;
	xport_filter_obj[filter_id].errno = 0;

	return &(xport_filter_obj[filter_id]);
}

CSDEMUX_RESULT CSDEMUX_Filter_Close(CSDEMUX_HANDLE handle)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	if(DEMUX_ENABLE == filter_ptr->filter_en){
		if(xport_filter_set_status(filter_ptr->filter_id,DEMUX_DISABLE)){
			printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
			return DEMUX_FAILED;
		}
	}

	xport_filter_clear_buffer(filter_ptr->filter_id);

	filter_ptr->filter_en = DEMUX_DISABLE;
	filter_ptr->errno = -1;

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_SetFilterType(CSDEMUX_HANDLE handle, CSDEMUX_FILTER_TYPE type)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	if(xport_filter_set_type(filter_ptr->filter_id, type)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_SetFilter(CSDEMUX_HANDLE handle, const unsigned char *const filter_ptr, const unsigned char *const mask_ptr)
{
	CSDEMUX_FILTER *filter_ptr_local = handle;

	CHECK_HANDLE_VALID(filter_ptr_local, CSDEMUX_OBJ_FILTER);

	if(xport_filter_set_filter(filter_ptr_local->filter_id, (unsigned char *)filter_ptr, (unsigned char *)mask_ptr)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_AddPID(CSDEMUX_HANDLE handle, unsigned short pid)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	if(xport_filter_set_pid(filter_ptr->filter_id, pid)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_ResetPID(CSDEMUX_HANDLE handle)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_ReadWait(CSDEMUX_HANDLE handle, unsigned int ms_timeout)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	return xport_filter_wait_data(filter_ptr->filter_id, ms_timeout);
}

CSDEMUX_RESULT CSDEMUX_Filter_CheckDataSize(CSDEMUX_HANDLE handle, unsigned int *size)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	*size = xport_filter_check_section_number(filter_ptr->filter_id);

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_GetSectionCount(CSDEMUX_HANDLE handle, unsigned int *count)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	*count = xport_filter_check_section_count(filter_ptr->filter_id);

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_ReadSectionData(CSDEMUX_HANDLE handle, unsigned char *buf, unsigned int *size)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	*size = xport_fliter_read_data(filter_ptr->filter_id, buf, *size);

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_Enable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	if(xport_filter_set_status(filter_ptr->filter_id, DEMUX_ENABLE)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}

CSDEMUX_RESULT CSDEMUX_Filter_Disable(CSDEMUX_HANDLE handle)
{
	CSDEMUX_FILTER *filter_ptr = handle;

	CHECK_HANDLE_VALID(filter_ptr, CSDEMUX_OBJ_FILTER);

	if(xport_filter_set_status(filter_ptr->filter_id, DEMUX_DISABLE)){
		printf("error: %s, %d. \n",__FUNCTION__,__LINE__);
		return DEMUX_FAILED;
	}

	return DEMUX_SUCCEED;
}


