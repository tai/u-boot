#ifndef _X_CLIB_H_
#define _X_CLIB_H_

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

void* x_memcpy(void *s1, const void *s2, int n);
int  x_memcmp(const void* s1, const void* s2, int n);
void* x_memset(void *s, int c, int n);
unsigned int udivsi3(unsigned int num, unsigned int den);

#endif
