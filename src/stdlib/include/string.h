#ifndef _STRING
#define _STRING

#ifndef _SIZET
#define _SIZET
typedef unsigned long long size_t;
#endif

int memcmp(const void* s1, const void* s2, size_t n);
#endif
