#ifndef __CTYPE_H__
# define __CTYPE_H__

/*
** Character classification functions. These are all implemented as sets of
** comparisons. There is an approach with a 128-byte table, but it takes more
** space; in fact, every function but isalnum, ispunct and isxdigit needs less
** code to compare than to perform the lookup. Additionally, users won't need
** the whole table for a single call to isspace().
*/
extern int isalnum(int c);
extern int isalpha(int c);
extern int isblank(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);

/* Case conversion functions */
extern int tolower(int c);
extern int toupper(int c);

/*
** Macro versions; these functions are very fast so it's worth it to inline
** them rather than perform a function call as expensive as the test. These are
** valid in the "C" locale, the only one supported by this library.
*/

#define isalnum(c) ({ \
	int __c0 = (c); \
	isalpha(__c0) || isdigit(__c0); \
})

#define isalpha(c) ({ \
	int __c = (c) | 0x20;  \
	(__c >= 'a') && (__c <= 'z'); \
})

#define isblank(c) ({ \
	int __c = (c); \
	(__c == 9) || (__c == 32); \
})

#define iscntrl(c) ({ \
	int __c = (c); \
	(__c < 32) || (__c >= 0x7f); \
})

#define isdigit(c) ({ \
	int __c = (c); \
	(__c >= '0') && (__c <= '9'); \
})

#define isgraph(c) ({ \
	int __c = (c); \
	(__c >= 33) && (__c < 0x7f); \
})

#define islower(c) ({ \
	int __c = (c); \
	(__c >= 'a') && (__c <= 'z'); \
})

#define isprint(c) ({ \
	int __c = (c); \
	(__c >= 32) && (__c < 0x7f); \
})

#define ispunct(c) ({ \
	int __c = (c); \
	(__c >= 33 && __c <= 47) || (__c >= 58 && __c <= 64) || \
	(__c >= 91 && __c <= 96) || (__c >= 123 && __c <= 126); \
})

#define isspace(c) ({ \
	int __c = (c); \
	(__c >= 9 && __c <= 13) || (__c == 32); \
})

#define isupper(c) ({ \
	int __c = (c); \
	(__c >= 'A') && (__c <= 'Z'); \
})

#define isxdigit(c) ({ \
	int __c = (c); \
	int __c20 = __c | 0x20; \
	(__c >= '0' && __c <= '9') || (__c20 >= 'a' && __c20 <= 'f'); \
})

#define tolower(c) ({ \
	int __c0 = (c); \
	isupper(__c0) ? (__c0 | 0x20) : __c0; \
})

#define toupper(c) ({ \
	int __c0 = (c); \
	islower(__c0) ? (__c0 & 0xdf) : __c0; \
})

#endif /*__CTYPE_H__*/
