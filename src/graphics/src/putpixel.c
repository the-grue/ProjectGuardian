#include <graphics.h>
#include <mmkernel.h>

#define SAFENSLOW 1

extern mmKernelTable *emktable;
extern unsigned int colors[];

void putpixel(unsigned long int x, unsigned long int y, unsigned int color)
{
#ifdef SAFENSLOW
	if(x > emktable->fb.Width || y > emktable->fb.Height)
		return;		// Out of bounds, do nothing
#endif
	*(unsigned int*)((x<<2) + emktable->yarray[y]) = colors[color];
}
