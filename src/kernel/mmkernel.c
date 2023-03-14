#include <mmkernel.h>
#include <sys8x8.h>
#include <sys8x16.h>

int _start(mmKernelTable *mmkerntab)
{
/*	return 0x600DB007;*/
	unsigned int y = 500;
	unsigned int BPP = 4;

	for(unsigned int x = 0; x < mmkerntab->fb.Width / 2 * BPP; x+=BPP)
		*(unsigned int *)(x + (y * mmkerntab->fb.PPScanLine * BPP) + mmkerntab->fb.FBBase) = 0xff00ff00;
	
	return mmkerntab->fb.FBBase;
}
