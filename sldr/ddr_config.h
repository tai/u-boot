//#define PLL_1600M
//#define PLL_1466M
//#define PLL_1404M
//#define PLL_1400M
//#define PLL_1333M
//#define PLL_1296M
#define PLL_1242M	//default.
//#define PLL_1188M
//#define PLL_1134M
//#define PLL_1080M
//#define PLL_1066M
//#define PLL_800M

#if defined(CONFIG_512M_DDR)

#if defined(PLL_1242M)  // action frequency is 1242MHz
	#define DDR_PLL         0x00001500 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xc6a28585
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#else	// CONFIG_512M_DDR

#if defined(PLL_1600M)  // action frequency is 1600MHz
	#define DDR_PLL         0x00001b00 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1466M)  // action frequency is 1466MHz
	#define DDR_PLL         0x00001a00 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1404M)  // action frequency is 1404MHz
	#define DDR_PLL         0x00001900 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1400M)  // action frequency is 1400MHz
	#define DDR_PLL         0x00001800 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1333M)  // action frequency is 891MHz
	#define DDR_PLL         0x00001700 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1296M)  // action frequency is 1296MHz
	#define DDR_PLL         0x00001600 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1242M)  // action frequency is 1242MHz
	#define DDR_PLL         0x00001500 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1188M)  // action frequency is 1188MHz
	#define DDR_PLL         0x00001400 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1134M)  // action frequency is 1134MHz
	#define DDR_PLL         0x00001300 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1080M)  // action frequency is 1080MHz
	#define DDR_PLL         0x00001200 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_1066M)  // action frequency is 1066MHz
	#define DDR_PLL         0x00001100 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#if defined(PLL_800M)  // action frequency is 891MHz
	#define DDR_PLL         0x00009c00 
	#define DDR_TIMING      0x87b820b1 
	#define DDR_MOD         0xb6514255 
	#define DDR_DELAY_0     0x3c3c3c3c
	#define DDR_DELAY_1     0x28182818
	#define DDR_MISC        0x7200c379 
	#define CLK_IMPEDANCE   0x2
	#define CMD_IMPEDANCE   0xe
#endif

#endif	// CONFIG_512M_DDR
