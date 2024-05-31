#include <graphics.h>
#include <mmkernel.h>

extern unsigned char *systemfont;
extern unsigned int cp_x, cp_y;
extern unsigned int ft_w, ft_h;
extern unsigned int currcolor, bgcolor;
extern mmKernelTable *emktable;
extern enum transparent_type trans_type;

void scroll(void);

void outtext(char *textstring)
{
	unsigned char ch;

	if(systemfont == NULL)
	{
		return;
	}

	while((ch = *textstring++) != '\0')
	{
		if((cp_x + ft_w) >= emktable->fb.Width)
		{
			cp_x = 0;
			cp_y += ft_h;
		}
		if((cp_y + ft_h) >= emktable->fb.Height)
		{
			scroll();
			cp_y -= ft_h;
		}
		for(int height = cp_y, hi = 0; hi < ft_h; height++, hi++)
		{
			for(int width = cp_x, lo = ft_w; lo >= 0; width++, lo--)
			{
				if(((systemfont[(ch * ft_h) + hi] >> lo) & 0x1) == 1)
					putpixel(width, height, currcolor);
				else 
					if(trans_type == OPAQUE)
						putpixel(width, height, bgcolor);
			}
		}
		cp_x = cp_x + ft_w;
	}
}

void scroll(void)
{
	for(unsigned long int y = ft_h; y < emktable->fb.Height; y++)
		for(unsigned long int x = 0; x < emktable->fb.Width; x++)
			*(unsigned int*)((x<<2) + emktable->yarray[y-ft_h]) = 
			*(unsigned int*)((x<<2) + emktable->yarray[y]);
	for(unsigned long int y = emktable->fb.Height - ft_h; y < emktable->fb.Height; y++)
		for(unsigned long int x = 0; x < emktable->fb.Width; x++)
			putpixel(x, y, BLACK);	
}

void outtextxy(unsigned int x, unsigned int y, char *textstring)
{
	unsigned int savex, savey;

	savex = cp_x;
	cp_x = x;
	savey = cp_y;
	cp_y = y;

	outtext(textstring);

	cp_x = savex;
	cp_y = savey;
}
