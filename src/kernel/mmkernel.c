#include <mmkernel.h>
#include <sys8x8.h>
#include <sys8x16.h>

unsigned int *mytab[8192];
extern unsigned int colors[];

int _start(mmKernelTable *mmkerntab)
{
	unsigned long int y = 400;
	mmkerntab->yarray = (unsigned int *)&mytab;

	// Generate Y-Lookup table to remove need for multiplications
	for(unsigned int i = 0; i < mmkerntab->fb.Height; i++){
		mmkerntab->yarray[i] = (unsigned long int)((mmkerntab->fb.PPScanLine * mmkerntab->fb.BPP * i) + mmkerntab->fb.FBBase);
	}

	// Draw some lines to make sure it is working both with Kernel Table values
	// as well as Y-Lookup table values
	for(unsigned long int x = 0; x < mmkerntab->fb.Width / 2 * mmkerntab->fb.BPP; x+=mmkerntab->fb.BPP){
		*(unsigned int *)(x + (y * mmkerntab->fb.PPScanLine * mmkerntab->fb.BPP) + mmkerntab->fb.FBBase) = 0xff00ffff;
		*(unsigned int *)(x + mmkerntab->yarray[y+10]) = 0xffff0000;
	}

	// Draw color ribbon at bottom reverse of top loaded by booter
	for(unsigned long int x = 0; x < 1024; x+=mmkerntab->fb.BPP)
                for(unsigned long int z = 0; z < 4; z++)
                        for(y = 500; y < 600; y++)
                                *((unsigned int *)(mmkerntab->yarray[y] + (mmkerntab->fb.BPP * mmkerntab->fb.PPScanLine) + 4 * (x+z))) = colors[255-(x/4)];

	
	return 0x600DB007;
}
