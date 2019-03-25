#include <common.h>
#include <netdev.h>


#ifdef CONFIG_DRIVER_CN100

/*
 * Initializes on-chip ethernet controllers.
 * to override, implement board_eth_init()
 */

int cpu_eth_init(bd_t *bis)
{
	return cn100_initialize(bis);
}
#endif
