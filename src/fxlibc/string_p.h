#ifndef __STRING_P_H__
# define __STRING_P_H__

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*
** Search for __needle within __haystack using either brute-force or the Knuth-
** Morris-Pratt algorithm. Ignores case if __ignore_case == true. This is the
** base function for strstr and strcasestr.
*/
char *__strstr_base(
	char const *__haystack,
	char const *__needle,
	bool __ignore_case);

#endif /*__STRING_P_H__*/
