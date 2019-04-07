#ifndef _C_STDLIB_H_
#define _C_STDLIB_H_

//#define USE_C_STD_LIB

#if defined(__CC_ARM)	//USE_C_STD_LIB
#include "x_clib.h"
#include <string.h>
//#include <string.h>
//#include <stdio.h>
//#include <assert.h>
//#include <stdlib.h>

#elif defined(__GNUC__)

#include "x_clib.h"

#define memcpy(s1, s2, n) x_memcpy(s1, s2, n)
#define memcmp(s1, s2, n) x_memcmp(s1, s2, n)
#define memset(s ,  c, n) x_memset(s ,  c, n)

#endif

#endif
