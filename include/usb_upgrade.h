#ifndef _USB_UPGRADE_
#define _USB_UPGRADE_


typedef struct
{
	unsigned int name_len;
	char *name;
}partition_file;

typedef struct
{
	unsigned int section_length;
	unsigned char digital_signature;
	unsigned int  manufactor_code;
	unsigned int  hardware_version;
	unsigned int  package_tool_version;
	unsigned int  minimum_uboot_version;
	unsigned int  create_date;
	unsigned int  create_time;
	unsigned char load_sequence_number;
	unsigned int  partition_count;
	partition_file *files;
}download_header;

typedef struct
{
	unsigned int section_length;
	unsigned char memory_type;
	unsigned char file_system_type;
	unsigned char erase_only;
	unsigned long long start_address;
	unsigned long long original_size;
	unsigned long long partition_size;
	unsigned int o_crc_32;
}partition_header;

typedef struct
{
	unsigned int name_len;
	char *name;
	unsigned int value_len;
	char *value;
}env_update_node;

typedef struct
{
	unsigned int section_length;
	unsigned int env_count;
	env_update_node *nodes;
}evn_update_st;

typedef struct
{
	unsigned int section_length;
	unsigned char arithmetic;
	unsigned int signature_len;
	char *signature_data;
}digital_signature_st;

typedef enum 
{
	SPI_FLASH_MEMORY_TYPE,
	NAND_FLASH_MEMORY_TYPE,
	EEPROM_MEMORY_TYPE,
	INVALID_MEMORY_TYPE,
}memory_type_e;

typedef enum 
{
	JFFS2_FILE_SYSTEM_TYPE,
	YAFFS_FILE_SYSTEM_TYPE,
	INVALID_FILE_SYSTEM_TYPE,
}filesystem_type_e;

typedef enum 
{
	SIGNATURE_ARITHMETIC_RSA_SHA1=1,
}signature_arithmetic_e;


#endif


