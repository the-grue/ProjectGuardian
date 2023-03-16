#ifndef _GRAPHICS
#define _GRAPHICS

#ifndef NULL
#define NULL 0
#endif

enum COLORS {
        BLACK,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        LIGHTGRAY,
        DARKGRAY,
        LIGHTBLUE,
        LIGHTGREEN,
        LIGHTCYAN,
        LIGHTRED,
        LIGHTMAGENTA,
        YELLOW,
        WHITE
};

void putpixel(unsigned long int x, unsigned long int y, unsigned int color);
void outtextxy(unsigned int x, unsigned int y, char *textstring);
void outtext(char *textstring);

#endif
