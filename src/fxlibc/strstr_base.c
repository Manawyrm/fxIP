#include "string_p.h"
#include <string.h>
#include <stdlib.h>
#include <fxlibc/ctype.h>

/* Quadratic-time brute-force algorithm. */
static char *strstr_bruteforce(char const *haystack, char const *needle,
	bool ignore_case)
{
	size_t len_needle = strlen(needle);

	while(*haystack) {
		int diff = ignore_case
			? strncasecmp(haystack, needle, len_needle)
			: strncmp(haystack, needle, len_needle);

		if(!diff) return (char *)haystack;
		haystack++;
	}

	return NULL;
}

/* Linear-time Knuth-Morris-Pratt algorithm. */
static char *strstr_kmp(char const *haystack, char const *needle, int16_t *T,
	bool ignore_case)
{
	/* Be careful, this macro has double evaluation */
	#define equal(c1, c2) \
		(ignore_case ? tolower(c1) == tolower(c2) : (c1) == (c2))

	/* Build the failure function */

	T[0] = -1;
	int cnd = 0;

	for(int pos = 1; needle[pos]; pos++, cnd++) {
		if(equal(needle[pos], needle[cnd])) {
			T[pos] = T[cnd];
		}
		else {
			T[pos] = cnd;
			while(cnd >= 0 && !equal(needle[pos], needle[cnd]))
				cnd = T[cnd];
		}
	}

	/* Perform linear-time matching */

	int i=0, pos=0;
	while(haystack[i]) {
		if(equal(needle[pos], haystack[i])) {
			pos++;
			i++;

			if(!needle[pos]) return (char *)haystack + i - pos;
		}
		else {
			pos = T[pos];
			if(pos < 0) {
				pos++;
				i++;
			}
		}
	}

	return NULL;
}

char *__strstr_base(char const *haystack, char const *needle,
	bool ignore_case)
{
	size_t len_needle = strlen(needle);
	int16_t *T = NULL;

	/*
	** Don't use the KMP algorithm for huge needles (> 64 kiB) as indexes
	** might not fit in an int16_t. (In any case, if you need to search for
	** such a huge pattern, don't use strstr.)
	*/
	if(len_needle < INT16_MAX) {
		T = malloc(len_needle * sizeof *T);
	}

	/* Fall back to bruteforce if allocation fails */
	if(T) {
		char *match = strstr_kmp(haystack, needle, T, ignore_case);
		free(T);
		return match;
	}
	else {
		return strstr_bruteforce(haystack, needle, ignore_case);
	}
}

