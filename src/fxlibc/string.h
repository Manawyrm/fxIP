#ifndef __STRING_H__
# define __STRING_H__

#include <stddef.h>

/* Copying functions. */

/* Copy __n characters from __src to __dest (overlap not allowed). */
extern void *memcpy(void *__dest, void const *__src, size_t __n);

/* Copy __n characters from __src to __dest (overlap allowed). */
extern void *memmove(void *__dest, void const *__src, size_t __n);

/* Copy string __src into __dest. */
extern char *strcpy(char * restrict __dest, char const * restrict __src);

/* Copy at most __n characters of __src into __dest. */
extern char *strncpy(char * restrict __dest, char const * restrict __src,
	size_t __n);

/* Concatenation functions. */

/* Copy __src at the end of __dest. */
extern char *strcat(char * restrict __dest, char const * restrict __src);

/* Copy at most __n characters of __src into __dest. */
extern char *strncat(char * restrict __dest, char const * restrict __src,
	size_t __n);

/* Comparison functions. */

/* Compare __n characters from __s1 and __s2. */
extern int memcmp(void const *__s1, void const *__s2, size_t __n);

/* Compare __s1 and __s2 as strings. */
extern int strcmp(char const *__s1, char const *__s2);

/* Compare __s1 and __s2 with locale-dependent collation rules. */
extern int strcoll(char const *__s1, char const *__s2);

/* Compare at most __n characters of __s1 and __s2. */
extern int strncmp(char const *__s1, char const *__s2, size_t __n);

/* Transform __src into __dest in a way that morphs strcoll into strcmp. */
extern size_t strxfrm(char * restrict __dest, char const * restrict __src,
	size_t __n);

/* Search functions. */

/* Search __c within the first __n characters of __s. */
extern void *memchr(void const *__s, int __c, size_t __n);

/* Find the first occurrence of __c within __s. */
extern char *strchr(char const *__s, int __c);

/* Count initial characters of __s that are not in __exclude. */
extern size_t strcspn(char const *__s, char const *__exclude);

/* Find the first character of __s that is also in __include. */
extern char *strpbrk(char const *__s, char const *__include);

/* Find the last occurrence of __c within __s. */
extern char *strrchr(char const *__s, int __c);

/* Count initial characters of __s that are in __include. */
extern size_t strspn(char const *__s, char const *__include);

/* Find the first occurrence of __s2 as a substring of __s1. */
extern char *strstr(char const *__s1, char const *__s2);

/* Break __s into tokens delimited by characters from __separators. */
extern char *strtok(char * restrict __s, char const * restrict __separators);

/* Miscellaneous functions. */

/* Fill the __n first bytes of __s with character __c. */
extern void *memset(void *__s, int __c, size_t __n);

/* Returns a string that describes the errno value __errnum. */
extern char *strerror(int __errnum);

/* Length of __s. */
extern size_t strlen(char const *__s);

/*
** Extensions
*/

/* The following functions are not specified by C99, but convenient. */

/* Minimum of __n and the length of __s. */
extern size_t strnlen(char const *__s, size_t __n);

/* Like strchr, but returns the end of __s if __c is not found. */
extern char *strchrnul(char const *__s, int __c);

/* Compare __s1 and __s2 case-insensitively. */
extern int strcasecmp(char const *__s1, char const *__s2);

/* Compare at most __n characters of __s1 and __s2 case-insensitively. */
extern int strncasecmp(char const *__s1, char const *__s2, size_t __n);

/* Find __needle in __haystack, ignoring case. */
extern char *strcasestr(char const *__haystack, char const *__needle);

/* Duplicate __s with malloc. */
extern char *strdup(char const *__s);

/* Duplicate at most __n characters of __s with malloc. */
extern char *strndup(char const *__s, size_t __n);

#endif /*__STRING_H__*/
