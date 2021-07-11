#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return (0);
	size_t i = -1;
	while (++i < n - 1 && s1[i] != '\0' && s2[i] != '\0'
	       && s1[i] == s2[i]) ;
	return (s1[i] - s2[i]);
}
