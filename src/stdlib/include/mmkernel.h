#ifndef _MMKERNEL
#define _MMKERNEL

#include <efi.h>

#ifndef _SIZET
#define _SIZET
typedef unsigned long long size_t;
#endif

typedef struct {
	void *FBBase;
	size_t FBSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PPScanLine;
	unsigned int BPP;
} framebuffer;

typedef struct {
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN MapSize;
	UINTN DescriptorSize;
} memmap;

typedef struct {
	framebuffer fb;
	memmap mmap;
	char *PAM;
	unsigned int *yarray;
} mmKernelTable;

#endif
