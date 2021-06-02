#include <string.h>
#include <fxlibc/ctype.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return (0);
	size_t i = -1;
	while (++i < n - 1 && s1[i] != '\0' && s2[i] != '\0'
	       && tolower(s1[i]) == tolower(s2[i])) ;
	return (tolower(s1[i]) - tolower(s2[i]));
}
