#include <string.h>
#ifndef __SUPPORT_ARCH_SH
void *memchr(void const *_s, int c, size_t n)
{
    char const *s = _s;

    for (size_t i = 0; i < n; i++)
    {
        if (s[i] == c)
            return (char *)&s[i];
    }

    return NULL;
}

#endif /*__SUPPORT_ARCH_SH*/
