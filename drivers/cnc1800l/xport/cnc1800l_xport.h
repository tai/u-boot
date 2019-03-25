#ifndef _CNC1800L_XPORT_H_
#define _CNC1800L_XPORT_H_

int xport_init(void);
int xport_set_channel_status(int chl_num,int status);
int xport_set_channel_inputmode(int chl_num,int input_mode);
int xport_pidft_set_status(int pidft_index,int status);
int xport_pidft_bind_chl(int chl_num,int pidft_index);
int xport_pidft_set_pid(int pidft_index,short int pid);
int xport_filter_clear_buffer(unsigned int filter_index);
int xport_filter_set_status(int filter_index, int status);
int xport_filter_set_type(int filter_index, int filter_type);
int xport_filter_set_filter(int filter_index, unsigned char *filter, unsigned char *mask);
int xport_filter_set_pid(unsigned int filter_index, unsigned int pid);
int xport_filter_wait_data(int filter_index, int ms_timeout);
int xport_filter_check_section_number(unsigned int filter_index);
int xport_filter_check_section_count(unsigned int filter_index);
int xport_fliter_read_data(int filter_index, unsigned char *buf, unsigned int size);

#endif

