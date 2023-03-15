#ifndef _MMKERNEL
#define _MMKERNEL

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
	framebuffer fb;
	unsigned int *yarray;
} mmKernelTable;

#endif
