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

enum fill_patterns {
        EMPTY_FILL,             // Fill with background color
        SOLID_FILL,             // Solid fill
        LINE_FILL,              // Fill with ---
        LTSLASH_FILL,           // Fill with ///, thin lines
        SLASH_FILL,             // Fill with ///, thick lines
        BKSLASH_FILL,           // Fill with \\\, thick lines
        LTBKSLASH_FILL,         // Fill with \\\, thin lines
        HATCH_FILL,             // Light hatch fill
        XHATCH_FILL,            // Heavy crosshatch fill
        INTERLEAVE_FILL,        // Interleaving line fill
        WIDE_DOT_FILL,          // Widely spaced dot fill
        CLOSE_DOT_FILL,         // Closely spaced dot fill
        USER_FILL               // User-defined fill pattern
};

struct fillsettingstype {
        int pattern;
        int color;
};

enum line_styles {
        SOLID_LINE      = 0,
        DOTTED_LINE     = 1,
        CENTER_LINE     = 2,
        DASHED_LINE     = 3,
        USERBIT_LINE    = 4
};

enum line_widths {
        NORM_WIDTH      = 1,
        THICK_WIDTH     = 3
};

struct linesettingstype {
        int linestyle;
        unsigned upattern;
        int thickness;
};

enum putimage_ops {
        COPY_PUT,
        XOR_PUT,
        OR_PUT,
        AND_PUT,
        NOT_PUT
};

struct viewporttype {
        int left, top, right, bottom;
        int clip;
};

enum text_just {
        LEFT_TEXT       = 0,            // Left justify text
        CENTER_TEXT     = 1,            // Center text horiz or vert
        RIGHT_TEXT      = 2,            // Right justify text
        BOTTOM_TEXT     = 0,            // Justify from bottom
        TOP_TEXT        = 2             // Justify from top
};

enum font_direction {
        HORIZ_DIR       = 0,            // Left to right
        VERT_DIR        = 1,            // Bottom to top
};

enum font_names {
        DEFAULT_FONT    = 0,            // 8x8 bit-mapped font
        TRIPLEX_FONT    = 1,            // Stroked triplex font
        SMALL_FONT      = 2,            // Stroked small font
        SANS_SERIF_FONT = 3,            // Stroked sans-serif font
        GOTHIC_FONT     = 4,            // Stroked gothic font
        SCRIPT_FONT     = 5,            // Stroked script font
        SIMPLEX_FONT    = 6,            // Stroked simplex font
        TRIPLEX_SCR_FONT= 7,            // Stroked triplex script font
        COMPLEX_FONT    = 8,            // Stroked complex font
        EUROPEAN_FONT   = 9,            // Stroked European font
        BOLD_FONT       = 10,           // Stroked bold font
};

struct textsettingstype {
        int font;
        int direction;
        int charsize;
        int horiz;
        int vert;
};

void putpixel(unsigned long int x, unsigned long int y, unsigned int color);
void outtextxy(unsigned int x, unsigned int y, char *textstring);
void outtext(char *textstring);
void line(int x1, int y1, int x2, int y2);

#endif
