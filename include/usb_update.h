#ifndef __USB_DEFS_H__
#define __USB_DEFS_H__

// musbmhdrc_cfg
// This file contains configuration constants for the musbmhdrc.

// ** Number of Tx endpoints **
// Legal values are 1 - 16 (this value includes EP0)
// ide, mouse, keyboard, 
#define C_NUM_EPT 4

// ** Number of Rx endpoints **
// Legal values are 1 - 16 (this value includes EP0)
#define C_NUM_EPR 4

// ** Endpoint 1 to 15 direction types **
// C_EP1_DEF is defined if either Tx endpoint 1 or Rx endpoint 1 are used
#define C_EP1_DEF

// C_EP1_TX_DEF is defined if Tx endpoint 1 is used
#define C_EP1_TX_DEF

// C_EP1_RX_DEF is defined if Rx endpoint 1 is used
#define C_EP1_RX_DEF

// C_EP1_TOR_DEF is defined if Tx endpoint 1 and Rx endpoint 1 share a FIFO
//#define C_EP1_TOR_DEF

// C_EP1_TAR_DEF is defined if both Tx endpoint 1 and Rx endpoint 1 are used
// and do not share a FIFO
#define C_EP1_TAR_DEF

// Similarly for all other used endpoints
#define C_EP2_DEF
#define C_EP2_TX_DEF
#define C_EP2_RX_DEF
#define C_EP2_TAR_DEF

#define C_EP3_DEF
#define C_EP3_TX_DEF
#define C_EP3_RX_DEF
#define C_EP3_TAR_DEF

// ** Endpoint 1 to 15 FIFO address bits **
// Legal values are 3 to 13 - corresponding to FIFO sizes of 8 to 8192 bytes.
// If an Tx endpoint shares a FIFO with an Rx endpoint then the Rx FIFO size
// must be the same as the Tx FIFO size.
// All endpoints 1 to 15 must be defined, unused endpoints should be set to 2.
#define C_EP1T_BITS 10
#define C_EP1R_BITS 10
#define C_EP2T_BITS 10
#define C_EP2R_BITS 10
#define C_EP3T_BITS 10
#define C_EP3R_BITS 10
#define C_EP4T_BITS 2
#define C_EP4R_BITS 2
#define C_EP5T_BITS 2
#define C_EP5R_BITS 2
#define C_EP6T_BITS 2
#define C_EP6R_BITS 2
#define C_EP7T_BITS 2
#define C_EP7R_BITS 2
#define C_EP8T_BITS 2
#define C_EP8R_BITS 2
#define C_EP9T_BITS 2
#define C_EP9R_BITS 2
#define C_EP10T_BITS 2
#define C_EP10R_BITS 2
#define C_EP11T_BITS 2
#define C_EP11R_BITS 2
#define C_EP12T_BITS 2
#define C_EP12R_BITS 2
#define C_EP13T_BITS 2
#define C_EP13R_BITS 2
#define C_EP14T_BITS 2
#define C_EP14R_BITS 2
#define C_EP15T_BITS 2
#define C_EP15R_BITS 2

// Define the following constants for ULPI PHY interface width.
#define C_LPI_WIDTH 7

// added by yan@10-25-2006 11:45 
#define C_LPI

// Define the following constant if software connect/disconnect control is required.
#define C_SOFT_CON

// Vendor control register widths.
#define C_VCTL_BITS 4
#define C_VSTAT_BITS 8

// Define the following constant to include a DMA controller.
#define C_DMA

// Define the following constant if 2 or more DMA channels are required.
#define C_DMA2

// ** Enable Dynamic FIFO Sizing **
#define C_DYNFIFO_DEF

// ** Derived constants **
// The following constants are derived from the previous configuration constants

// Total number of endpoints
// Legal values are 2 - 16
// This must be equal to the larger of C_NUM_EPT, C_NUM_EPR
#define C_NUM_EPS 4

// C_EPMAX_BITS is equal to the largest endpoint FIFO word address bits
#define C_EPMAX_BITS 9

// C_RAM_BITS is the number of address bits required to address the RAM (32-bit addresses)
// It is defined as log2 of the sum of 2** of all the endpoint FIFO dword address bits (rounded up).
// 2 x 1024 to support any possible ep(max bulk 512'B, max iso = 1024'B)
#define C_RAM_BITS 9

#define SOF 0Xa5
#define SETUP 0X2d
#define DATA0 0Xc3
#define DATA1 0X4b
#define DATA2 0X87
#define MDATA 0X0f
#define ACK 0Xd2
#define NAK 0X5a
#define STALL 0X1e
#define PING 0Xb4
#define NYET 0X96
#define SPLIT 0X78
#define ERR 0X3c
#define PRE 0X3c

// MCU registers
#define REG_FADDR	 0X00
#define REG_POWER	 0X01
#define REG_INTRTX	 0X02
#define REG_INTRRX	 0X04
#define REG_INTRTXE	 0X06
#define REG_INTRRXE	 0X08
#define REG_INTRUSB	 0X0A
#define REG_INTRUSBE	 0X0B
#define REG_FRAME        0X0C
#define REG_INDEX        0X0E
#define REG_TESTMODE     0X0F
#define REG_EP0_CSR	 0X12
#define REG_EP0_COUNT	 0X18
#define REG_EP0_INTV     0X1B
#define REG_EP_TXMAXP	 0X10
#define REG_EP_TXCSR	 0X12
#define REG_EP_RXMAXP    0X14
#define REG_EP_RXCSR	 0X16
#define REG_EP_COUNT	 0X18
#define REG_EP_TXTYPE    0X1A
#define REG_EP_TXINTV    0X1B
#define REG_EP_RXTYPE    0X1C
#define REG_EP_RXINTV    0X1D
#define REG_EP_FIFOBITS  0X1F
#define REG_EP0_FIFO	 0X20
#define REG_EP1_FIFO	 0X24
#define REG_EP2_FIFO	 0X28
#define REG_EP3_FIFO	 0X2C
#define REG_EP4_FIFO	 0X30
#define REG_EP5_FIFO	 0X34
#define REG_EP6_FIFO	 0X38
#define REG_EP7_FIFO	 0X3C
#define REG_EP8_FIFO	 0X40
#define REG_EP9_FIFO	 0X44
#define REG_EP10_FIFO    0X48
#define REG_EP11_FIFO    0X4C
#define REG_EP12_FIFO    0X50
#define REG_EP13_FIFO    0X54
#define REG_EP14_FIFO	 0X58
#define REG_EP15_FIFO    0X5C
#define REG_DEVCTL	 0X60
#define REG_EP_TXFIFOS   0X62
#define REG_EP_RXFIFOS   0X63
#define REG_EP_TXFIFOA   0X64
#define REG_EP_RXFIFOA   0X66
#define REG_VCNTRL	 0X68
#define REG_VSTAT	 0X68
#define REG_HWVERS	 0X6C
#define REG_LPI_VBUS     0X70
#define REG_LPI_CARCTRL  0X71
#define REG_LPI_IMASK    0X72
#define REG_LPI_ISTAT    0X73
#define REG_LPI_DATA     0X74
#define REG_LPI_ADDR     0X75
#define REG_LPI_CTRL     0X76
#define REG_LPI_RXDATA   0X77
#define REG_ENDCOUNT   	 0X78
#define REG_RAMSIZE	     0X79
#define REG_PHYWT        0X7A
#define REG_PHYTMVBUS    0X7B
#define REG_TMHSEOF1     0X7C
#define REG_TMFSEOF1     0X7D
#define REG_TMLSEOF1     0X7E

// Endpoint target address off
#define REG_TXTFADDR_OFF 0
#define REG_TXTHADDR_OFF 2
#define REG_TXTHPORT_OFF 3
#define REG_RXTFADDR_OFF 4
#define REG_RXTHADDR_OFF 6
#define REG_RXTHPORT_OFF 7

// Endpoint register offsets
#define REG_EP0_OFF    0X0100
#define REG_EP1_OFF    0X0110
#define REG_EP2_OFF    0X0120
#define REG_EP3_OFF    0X0130
#define REG_EP4_OFF    0X0140
#define REG_EP5_OFF    0X0150
#define REG_EP6_OFF    0X0160
#define REG_EP7_OFF    0X0170
#define REG_EP8_OFF    0X0180
#define REG_EP9_OFF    0X0190
#define REG_EP10_OFF   0X01A0
#define REG_EP11_OFF   0X01B0
#define REG_EP12_OFF   0X01C0
#define REG_EP13_OFF   0X01D0
#define REG_EP14_OFF   0X01E0
#define REG_EP15_OFF   0X01F0

// DMA controller registers
#define REG_DMA_INTR   0x200
#define REG_DMA_CNTRL 0x20C
#define REG_DMA_ADDR   0x208
#define REG_DMA_COUNT  0x204

// Interrupt status bits
#define INTR_EP0	0X0001
#define INTR_EP1	0X0002
#define INTR_EP2	0X0004
#define INTR_EP3	0X0008
#define INTR_EP4	0X0010
#define INTR_EP5	0X0020
#define INTR_EP6	0X0040
#define INTR_EP7	0X0080
#define INTR_EP8	0X0100
#define INTR_EP9	0X0200
#define INTR_EP10	0X0400
#define INTR_EP11	0X0800
#define INTR_EP12	0X1000
#define INTR_EP13	0X2000
#define INTR_EP14	0X4000
#define INTR_EP15	0X8000
#define INTR_SUS	0X01
#define INTR_RES	0X02
#define INTR_RST	0X04
#define INTR_SOF	0X08
#define INTR_CONN	0X10
#define INTR_DISCON	0X20
#define INTR_SREQ	0X40
#define INTR_VBERR	0X80

// CSR0 bits
#define CSR0_NULL	0X00
#define CSR0_OPR	0X01
#define CSR0_IPR	0X02
#define CSR0_SNTS	0X04
#define CSR0_DE		0X08
#define CSR0_SE		0X10
#define CSR0_SS		0X20
#define CSR0_SVDOPR	0X40
#define CSR0_SVDSE	0X80
#define CSR0_RPR	0X01
#define CSR0_TPR	0X02
#define CSR0_RSTALL	0X04
#define CSR0_SETUP	0X08
#define CSR0_ERR	0X10
#define CSR0_REQP	0X20
#define CSR0_STATP	0X40
#define CSR0_NAKL	0X80
#define CSR0H_FF	0X01
#define CSR0H_DISPING	0X08

// TXCSR1 bits
#define TXCSR1_NULL	0X00
#define TXCSR1_STALL	0X20
#define TXCSR1_TPR	0X01
#define TXCSR1_IPR	0X01
#define TXCSR1_FNE	0X02
#define TXCSR1_UR	0X04
#define TXCSR1_ERR	0X04
#define TXCSR1_FF	0X08
#define TXCSR1_SDS	0X10
#define TXCSR1_SETUP 0X10
#define TXCSR1_STS	0X20
#define TXCSR1_RSTL	0X20
#define TXCSR1_CDT	0X40
#define TXCSR1_ITX	0X80
#define TXCSR1_NAKL	0X80
#define TXCSR1_TPRX	0	//8'b0000000X
#define TXCSR1_IPRX	0	//8'b0000000X
#define TXCSR1_FNEX	2	//8'b0000001X

// TXCSR2 bits
#define TXCSR2_NULL	0X00
#define TXCSR2_DTOG 	0X01
#define TXCSR2_DTOGWRE 	0X02
#define TXCSR2_DMAMODE	0X04
#define TXCSR2_FDT	0X08
#define TXCSR2_DMA	0X10
#define TXCSR2_MODE	0X20
#define TXCSR2_ISO	0X40
#define TXCSR2_AUTO	0X80
#define TXCSR2_DTX 	0	//8'b0000000X

// RXCSR1 bits
#define RXCSR1_NULL	0X00
#define RXCSR1_RPR	0X01
#define RXCSR1_OPR	0X01
#define RXCSR1_FULL	0X02
#define RXCSR1_OR	0X04
#define RXCSR1_ERR	0X04
#define RXCSR1_DE	0X08
#define RXCSR1_NAKL	0X08
#define RXCSR1_FF	0X10
#define RXCSR1_SDS	0X20
#define RXCSR1_REQP	0X20
#define RXCSR1_STS	0X40
#define RXCSR1_RSTL	0X40
#define RXCSR1_CDT	0X80
#define RXCSR1_RPRX	1	//8'b000000X1
#define RXCSR1_OPRX	1	//8'b000000X1
#define RXCSR1_INCMPRX	0	//8'bXXXXXXX0

// RXCSR2 bits
#define RXCSR2_NULL	0X00
#define RXCSR2_IRX	0X01
#define RXCSR2_DTOG	0X02
#define RXCSR2_DTOGWRE 0X04
#define RXCSR2_DMAMODE 0X08
#define RXCSR2_DISNYET 0X10
#define RXCSR2_PIDERR  0X10
#define RXCSR2_DMA	0X20
#define RXCSR2_ISO	0X40
#define RXCSR2_AREQ 0X40
#define RXCSR2_AUTO	0X80

// Protocol type bits
#define TYPE_CNTRL 0X00
#define TYPE_ISOC	0X10
#define TYPE_BULK	0X20
#define TYPE_INTR	0X30
#define TYPE_HS 0X40
#define TYPE_FS 0X80
#define TYPE_LS 0Xc0

#define DFIFODPB 0X10
#define REG_NULL 0X00

// Power register bits
#define PWR_NULL        0X00
#define PWR_SUSE        0X01
#define PWR_SUSM        0X02
#define PWR_RSM         0X04
#define PWR_RST         0X08
#define PWR_HSMOD       0X10
#define PWR_HSEN        0X20
#define PWR_SFTCON      0X40

// DEVCTL bits
#define DVC_NULL       0X00
#define DVC_SESS       0X01
#define DVC_HREQ       0X02
#define DVC_HMODE      0X04
#define DVC_VBUS       0X18
#define DVC_LSDEV      0X20
#define DVC_FSDEV      0X40
#define DVC_CID        0X80
#define DVC_CIDX      0	// 8'bX0000000

// TESTMODE bits
#define TEST_SE0_NAK    0X01
#define TEST_J          0X02
#define TEST_K          0X04
#define TEST_PACKET     0X08
#define TEST_FORCE_HS   0X10
#define TEST_FORCE_FS   0X20
#define TEST_FIFO_ACC   0X40
#define TEST_FORCE_HOST 0X80

// ULPI VBUS control bits
#define LPI_VBUS_NULL   0X00
#define LPI_VBUS_EXT    0X01
#define LPI_VBUS_IND    0X02

// Define ULPI Function control register bits
#define LPI_FC_SUSPENDM  0X40

// Define ULPI Interface control register bits
#define LPI_IF_FSLSMODE6  0X01
#define LPI_IF_FSLSMODE3  0X02
#define LPI_IF_CARKIT     0X04
#define LPI_IF_CLKSUPSEND 0X08

// ULPI register bits and internal addresses
#define LPI_NULL          0X00
#define LPI_REQ           0X01
#define LPI_COMPLT        0X02
#define LPI_RD            0X04

// Car Kit Control Bits.
#define LPI_C_DISABLE     0X01
#define LPI_C_ACTIVE      0X02
#define LPI_C_ALTINT      0X04
#define LPI_C_WAKEUP      0X08
#define LPI_C_RXCMD       0X10
#define LPI_C_ACTIVEEND   0X20
// Car Kit IMASK Register
#define LPI_I_MCU_DONE    0X01
#define LPI_I_ALTINT      0X02
#define LPI_I_CAREXIT     0X04
#define LPI_I_RXCMD       0X08

#define ULPI_VEND_LO      0X00
#define ULPI_VEND_HI      0X01
#define ULPI_PROD_LO      0X02
#define ULPI_PROD_HI      0X03
#define ULPI_FUN_CTL      0X04
#define ULPI_IF_CTL       0X07
#define ULPI_OTG_CTL      0X0A
#define ULPI_INT_STA     0x13
#define ULPI_INT_LATCH 0x14
#define ULPI_DEBUG      0x15
#define ULPI_SCRATCH      0X16
#define ULPI_CAR_CTL      0X19
#define ULPI_CAR_INT_DEL  0X1C
#define ULPI_CAR_INT_EN   0X1D
#define ULPI_CAR_INT_ST   0X20
#define ULPI_CAR_INT_LA   0X21
#define ULPI_CAR_PULSE    0X22
#define ULPI_TX_POS_WID   0X25
#define ULPI_TX_NEG_WID   0X26
#define ULPI_RX_POLARITY  0X27



#define ULPI_POW_CTRL     0x3D

// Reset values for timers
#define RST_REG_PHYWT     0X5C
#define RST_REG_PHYTMVBUS 0X3C
#define RST_REG_TMHSEOF1  0X80
#define RST_REG_TMFSEOF1  0X77
#define RST_REG_TMLSEOF1  0X72

// Packet Buffer bits
#define SINGLE          0
#define DOUBLE          1
#define EP0_OFFSET     0X040       // Initial 64 offset for EP0
#define FIFOEIGHT       0      // Size bits for 8 byte FIFO
#define FIFOSIXTEEN     1      // Size bits for 16 byte FIFO
#define FIFOTHREETWO    2      // Size bits for 32 byte FIFO
#define FIFOSIXFOUR     3      // Size bits for 64 byte FIFO
#define EIGHT           0X08
#define SIXTEEN         0X10
#define THREETWO        0X20
#define SIXFOUR         0X40

// DMA Controller bits
#define DMACNTRL_ENAB   0X0001
#define DMACNTRL_DIR    0X0002
#define DMACNTRL_MODE   0X0004
#define DMACNTRL_INTE   0X0008
#define DMACNTRL_ERR    0X0100
#define DMACNTRL_INCR16 0X0600
#define DMACNTRL_INCR8  0X0400
#define DMACNTRL_INCR4  0X0200

// MCU byte enables
#define BE_8_0          1	// byte 0
#define BE_8_1          2	// byte 1
#define BE_8_2          4	// byte 2
#define BE_8_3          8	// byte 3
#define BE_16L		3	// lower word
#define BE_16H		12	// upper word
#define BE_32		15	// full 32-bits

// MCU access sizes
#define SIZE8		0
#define SIZE16	1
#define SIZE32	2

// FIFO load modes

#define FIFO_8LOW    0	// 8-bit low byte of bus
#define FIFO_8CYC    1	// 8-bit cycle through bytes on bus
#define FIFO_16LOW   2	// 16-bit low word of bus
#define FIFO_16CYC   3	// 16-bit cycle through words on bus
#define FIFO_32      4	// 32-bit
#define FIFO_32IDLEs 5 // 32-bit with interleaving IDLE cycles

// DMA burst modes
#define DMABURST_INCR   0
#define DMABURST_INCR4  1
#define DMABURST_INCR8  2
#define DMABURST_INCR16 3

//Split token fields
#define SPT_SSPLIT 0
#define SPT_CSPLIT 1
#define SPT_FS 0
#define SPT_LS 1
#define SPT_ALL 3
#define SPT_BEG 1
#define SPT_MID 0
#define SPT_END 2

// AHB address of MCUTxBuff
#define AHB_ADDR_TXBUF  0X00010000

// AHB address of MCURxBuff
#define AHB_ADDR_RXBUF  0X00020000
#define CYC (HCYC*2)		// Period of USB system clock
#define HOLDDELAY (HCYC*5)	// Delay for gate level simulations
#define AUTOACKDELAY 0.100  	// Delay for checking AutoAckEnab1 in wait_till_ack_sent task
#define CHKCNTSDELAY 130.000 // Delay used in task CheckContinuous
#define HCLK (HCLK0_CYC / 2)	

#define HDEL (HCLK/2)           // AHB bus 'delay'
#define CLK (HCLK*2)		// Period of CPU clock
#define HSBIT (HCYC*2)		// USB high-speed bit period
#define FSBIT (HCYC*80)	// USB full-speed bit period
#define LSBIT (HCYC*640)	// USB low-speed bit period
#define CHIRP (HSBIT*1216)	// HS detection chirp length (from host)
#define DISPPER (HCYC*2)	// Output file sample period
#define STROBE 0.0009		// Strobe period (offset from system clock)
#define MICROSEC (HCYC*960)    // 1 micro second
#define TBCHIRP (16*MICROSEC)  // Testbench auto chirp time
#define MAX_WAIT_LNST (600*FSBIT) // Maximum time to wait for Linestate change from phy
#define MAX_WAIT_FOR_INT (200 * HCLK)  // maximum time to wait for MCU interrupt
#define MAX_TO_MC_NINT (200 * HCLK)  // maximum time to wait for MCU interrupt
#define MAX_CORE_REG_UPDATE (UTM_CLK*8)  // maximum time for core to update it's registers

#define CORE_ACTION_TIME (CLK*30)// Max time for register update to trigger request

#define MAX_LS_NOTO (LSBIT*16)   // Max ip delay for no time out
#define MIN_LS_TO (LSBIT*18)     // Minimum ip delay to give timeout
#define MIN_LSIP_DEL (LSBIT*2)   // Minimum low-speed inter-packet delay
#define TYP_LSIP_DEL (LSBIT*4)   // Typical low-speed inter-packet delay
#define MAX_LSIP_DEL (LSBIT*7.5) // Maximum low-speed inter-packet delay (don't use!)
#define MAX_LSHIP_DEL (LSBIT*7.5)// Maximum low-speed inter-packet delay for host response
#define MAX_LSDIP_DEL (LSBIT*6.5)// Maximum low-speed inter-packet delay for device response
#define LS_HNDSK_LEN (LSBIT*18)  // low speed handshake length (sync+pid+EOP)
#define LS_TOKEN_LEN (LSBIT*36)  // low speed token length (sync+3 bytes+max 2 bits stuff+EOP)
#define FS_PREAMBLE_LEN (FSBIT*21)  // Full speed preamble length (sync+1 + HSU)
#define MAX_LS_PKT_LEN_TO (LSBIT*8*15) // Timeout from start of packet to end of packet


#define MAX_FS_NOTO (FSBIT*16)   // Max ip delay for no time out
#define MIN_FS_TO (FSBIT*18)     // Minimum ip delay to give timeout
#define MIN_FSIP_DEL (FSBIT*2)   // Minimum full-speed inter-packet delay
#define TYP_FSIP_DEL (FSBIT*4)   // Typical full-speed inter-packet delay
#define MAX_FSIP_DEL (FSBIT*7.5) // Maximum full-speed inter-packet delay (don't use!)
#define MAX_FSHIP_DEL (FSBIT*7.5)// Maximum full-speed inter-packet delay for host response
#define MAX_FSDIP_DEL (FSBIT*6.5)// Maximum full-speed inter-packet delay for device response
#define FS_HNDSK_LEN (FSBIT*18)  // Full speed handshake length (sync+pid+EOP)
#define FS_TOKEN_LEN (FSBIT*36)  // Full speed token length (sync+3 bytes+max 2 bits stuff+EOP)
#define MAX_FS_PKT_LEN_TO (FSBIT*8*1500) // Timeout from start of packet to end of packet

#define MAX_HS_NOTO (HSBIT*734)   // Max ip delay for no time out
#define MIN_HS_TO (HSBIT*816)     // Minimum ip delay to give timeout
#define MIN_HSIP_DEL (HSBIT*8)   // Minimum high-speed inter-packet delay (different direction)
#define MIN_HSIP_DEL_TX_TX (HSBIT*88)  // Minimum high-speed inter-packet delay (same direction)
#define MIN_HSIT_DEL (HSBIT*32)  // Minimum high-speed inter-transaction delay
#define TYP_HSIP_DEL (HSBIT*176) // Typical high-speed inter-packet delay
#define MAX_HSIP_DEL (HSBIT*192) // Maximum high-speed inter-packet delay
#define HS_HNDSK_LEN (HSBIT*48)  // High speed handshake length (32 bits sync+pid+EOP)
#define HS_TOKEN_LEN (HSBIT*66)  // High speed token length (32 bits sync+3 bytes+max 2 bits stuff+EOP)
#define HS_SOF_LEN (HSBIT*98)  // High speed token length (32 bits sync+3 bytes+max 2 bits stuff+long EOP)
#define MAX_HS_PKT_LEN_TO (HSBIT*8*1500) // Timeout from start of packet to end of packet

#define IDPULLUP_DEL (HSBIT*7500) // Delay for IDDIG to be valid (>15 us)

// Speed of target device
#define SPEED_HIGH 0X40
#define SPEED_FULL 0X80
#define SPEED_LOW 0Xc0

#define TXCSR_MODE 0X2000
#define TXCSR_TXPKTRDY 0X0001
#define DFIFODPB 0X10
#define REG_NULL 0X00

#define REG_TXFUNCADDR_EP0 0X80
#define REG_RXFUNCADDR_EP0 0X84
#define REG_TXFUNCADDR_EP1 0X88
#define REG_RXFUNCADDR_EP1 0X8c

#define REG_TXFUNCADDR_EP2 0X90
#define REG_RXFUNCADDR_EP2 0X94
#define REG_TXFUNCADDR_EP3 0X98
#define REG_RXFUNCADDR_EP3 0X9c

#define REG_TXFUNCADDR_EP4 0Xa0
#define REG_RXFUNCADDR_EP4 0Xa4
#define REG_TXFUNCADDR_EP5 0Xa8
#define REG_RXFUNCADDR_EP5 0Xac

#define REG_TXFUNCADDR_EP6 0Xb0
#define REG_RXFUNCADDR_EP6 0Xb4
#define REG_TXFUNCADDR_EP7 0Xb8
#define REG_RXFUNCADDR_EP7 0Xbc

#define REG_TXFUNCADDR_EP8 0Xc0
#define REG_RXFUNCADDR_EP8 0Xc4
#define REG_TXFUNCADDR_EP9 0Xc8
#define REG_RXFUNCADDR_EP9 0Xcc

#define REG_TXFUNCADDR_EP10 0Xd0
#define REG_RXFUNCADDR_EP10 0Xd4
#define REG_TXFUNCADDR_EP11 0Xd8
#define REG_RXFUNCADDR_EP11 0Xdc

#define REG_TXFUNCADDR_EP12 0Xe0
#define REG_RXFUNCADDR_EP12 0Xe4
#define REG_TXFUNCADDR_EP13 0Xe8
#define REG_RXFUNCADDR_EP13 0Xec

#define REG_TXFUNCADDR_EP14 0Xf0
#define REG_RXFUNCADDR_EP14 0Xf4
#define REG_TXFUNCADDR_EP15 0Xf8
#define REG_RXFUNCADDR_EP15 0Xfc

#define REG_RXHUBADDR_EP0 0X86
#define REG_RXHUBADDR_EP1 0X8e
#define REG_RXHUBADDR_EP2 0X96
#define REG_RXHUBADDR_EP3 0X9e
#define REG_RXHUBADDR_EP4 0Xa6
#define REG_RXHUBADDR_EP5 0Xae
#define REG_RXHUBADDR_EP6 0Xb6
#define REG_RXHUBADDR_EP7 0Xbe
#define REG_RXHUBADDR_EP8 0Xc6
#define REG_RXHUBADDR_EP9 0Xce
#define REG_RXHUBADDR_EP10 0Xd6
#define REG_RXHUBADDR_EP11 0Xde
#define REG_RXHUBADDR_EP12 0Xe6
#define REG_RXHUBADDR_EP13 0Xee
#define REG_RXHUBADDR_EP14 0Xf6
#define REG_RXHUBADDR_EP15 0Xfe

#define REG_TXHUBADDR_EP0 0X82
#define REG_TXHUBADDR_EP1 0X8a
#define REG_TXHUBADDR_EP2 0X92
#define REG_TXHUBADDR_EP3 0X9a
#define REG_TXHUBADDR_EP4 0Xa2
#define REG_TXHUBADDR_EP5 0Xaa
#define REG_TXHUBADDR_EP6 0Xb2
#define REG_TXHUBADDR_EP7 0Xba
#define REG_TXHUBADDR_EP8 0Xc2
#define REG_TXHUBADDR_EP9 0Xca
#define REG_TXHUBADDR_EP10 0Xd2
#define REG_TXHUBADDR_EP11 0Xda
#define REG_TXHUBADDR_EP12 0Xe2
#define REG_TXHUBADDR_EP13 0Xea
#define REG_TXHUBADDR_EP14 0Xf2
#define REG_TXHUBADDR_EP15 0Xfa

#define REG_TXHUBPORT_EP0 0X83
#define REG_TXHUBPORT_EP1 0X8b
#define REG_TXHUBPORT_EP2 0X93
#define REG_TXHUBPORT_EP3 0X9b
#define REG_TXHUBPORT_EP4 0Xa3
#define REG_TXHUBPORT_EP5 0Xab
#define REG_TXHUBPORT_EP6 0Xb3
#define REG_TXHUBPORT_EP7 0Xbb
#define REG_TXHUBPORT_EP8 0Xc3
#define REG_TXHUBPORT_EP9 0Xcb
#define REG_TXHUBPORT_EP10 0Xd3
#define REG_TXHUBPORT_EP11 0Xdb
#define REG_TXHUBPORT_EP12 0Xe3
#define REG_TXHUBPORT_EP13 0Xeb
#define REG_TXHUBPORT_EP14 0Xf3
#define REG_TXHUBPORT_EP15 0Xfb

#define REG_RXHUBPORT_EP0 0X87
#define REG_RXHUBPORT_EP1 0X8f
#define REG_RXHUBPORT_EP2 0X97
#define REG_RXHUBPORT_EP3 0X9f
#define REG_RXHUBPORT_EP4 0Xa7
#define REG_RXHUBPORT_EP5 0Xaf
#define REG_RXHUBPORT_EP6 0Xb7
#define REG_RXHUBPORT_EP7 0Xbf
#define REG_RXHUBPORT_EP8 0Xc7
#define REG_RXHUBPORT_EP9 0Xcf
#define REG_RXHUBPORT_EP10 0Xd7
#define REG_RXHUBPORT_EP11 0Xdf
#define REG_RXHUBPORT_EP12 0Xe7
#define REG_RXHUBPORT_EP13 0Xef
#define REG_RXHUBPORT_EP14 0Xf7
#define REG_RXHUBPORT_EP15 0Xff

#define EP_NUM_EP0  0x0		/* Endpoint number */

#endif /* end of __USB_DEFS_H__ */

