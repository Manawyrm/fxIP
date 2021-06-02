#include <string.h>
#include "string_p.h"

char *strcasestr(char const *haystack, char const *needle)
{
	return __strstr_base(haystack, needle, true);
}
