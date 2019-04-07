/*
// Author: Jiasheng.chen@celestialsemi.com
// Version0.1 Inital @2008.02.15
*/

#include <x_clib.h>

void *x_memset(
   void* dest,
   int c, 
   int count )
{
	int i = 0;
	if (dest == NULL || count <=0)
	{
		return NULL;
	}
	while (i < count)
	{
		*((char *)dest + i) = (char)c;
		i++;
	}
	return dest;
}

void *x_memcpy(
   void *dest,
   const void *src,
   int count 
)
{
	int i = 0;
	if (count <=0)
	{
		return NULL;
	}
	for (i = 0; i < count ; i++)
	{
		*((char *)dest + i) = *((char *)src + i); 
	}
	return dest;
}

int  x_memcmp(const void* str1, const void* str2, int n)
{
    register unsigned char *s1 = (unsigned char*)str1;
    register unsigned char *s2 = (unsigned char*)str2;
	if (s1 == NULL || s2 == NULL)
		return -1;
    while (n-- > 0) {
        if (*s1++ != *s2++)
            return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}

unsigned int udivsi3(unsigned int num, unsigned int den)
{
    unsigned int quot = 0, qbit = 1;

    if ( den == 0 ) {
        ///__divide_error();
        return 0;			/* If trap returns... */
    }

    /* Left-justify denominator and count shift */
    while ( (int)den >= 0 ) {
        den <<= 1;
        qbit <<= 1;
    }

    while ( qbit ) {
        if ( den <= num ) {
            num -= den;
            quot += qbit;
        }
        
        den >>= 1;
        qbit >>= 1;
    }

    return quot;
}

