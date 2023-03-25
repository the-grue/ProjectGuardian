#include <mmkernel.h>
#include <graphics.h>
#include <sys8x8.h>
#include <sys8x16.h>

unsigned int *mytab[8192];
extern unsigned int colors[];
mmKernelTable *emktable;
unsigned char *systemfont;
unsigned int cp_x, cp_y, ft_w, ft_h;
unsigned int currcolor;
unsigned int bgcolor;
struct linesettingstype linesettingstype;
unsigned short line_patterns[5] = { 0xFFFF, 0xCCCC, 0xFC78, 0xF8F8, 0xFFFF };

int _start(mmKernelTable *mmkerntab)
{
	emktable = mmkerntab;
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

	for(unsigned long int x = 0; x < mmkerntab->fb.Width / 2; x++)
		putpixel(x, y+20, 15);

	// Draw color ribbon at bottom reverse of top loaded by booter
	// using putpixel
	for(unsigned long int x = 0; x < 1024; x+=mmkerntab->fb.BPP)
                for(unsigned long int z = 0; z < 4; z++)
                        for(y = 500; y < 600; y++)
				putpixel(x+z, y, 255-(x/4));

	// Try outtext and outtextxy
	// With 8x8 font and 8x16 font
	ft_w = 8;
	ft_h = 8;
	cp_x = 300;
	cp_y = 600;
	systemfont = sysfont_8x8;
	currcolor = RED;
	bgcolor = BLACK;
	outtext("Project Guardian");
	currcolor = WHITE;
	outtextxy(298, 598, "Project Guardian");

	cp_x = 0;
	cp_y = 616;
	for(unsigned int count = 0; count < 16; count++)
	{
		currcolor = count % 16;
		if(currcolor == WHITE)
			bgcolor = BLACK;
		else
			bgcolor = (count + 1) % 16;
		outtext("Project Guardian ");
	}

	cp_x = 300;
	cp_y = 650;
	ft_h = 16;
	systemfont = sysfont_8x16;
	currcolor = BLUE;
	outtext("Project Guardian");
	currcolor = WHITE;
	outtextxy(298, 648, "Project Guardian");

	cp_x = 0;
	cp_y = 670;
	for(unsigned int count = 0; count < 50; count++)
	{
		currcolor = count % 16;
		if(currcolor == WHITE)
			bgcolor = BLACK;
		else
			bgcolor = WHITE;
		outtext("Project Guardian ");
	}

	linesettingstype.linestyle = SOLID_LINE;
	linesettingstype.upattern = 0;
	linesettingstype.thickness = THICK_WIDTH;;
	currcolor = RED;
	line(0, 0, mmkerntab->fb.Width - 1, mmkerntab->fb.Height - 1);
	currcolor = YELLOW;
	line(0, mmkerntab->fb.Height - 1, mmkerntab->fb.Width - 1, 0);
	linesettingstype.thickness = NORM_WIDTH;
	currcolor = YELLOW;
	line(0, 0, mmkerntab->fb.Width - 1, mmkerntab->fb.Height - 1);
	currcolor = RED;
	line(0, mmkerntab->fb.Height - 1, mmkerntab->fb.Width - 1, 0);


	return 0x600DB007;
}
