#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdlib.h>
#include "ansi-terminal/Ansi.h"
#include <string.h>

typedef struct v2i{int x, y;}v2i_t;
typedef struct color_s{char r; char g; char b}color_t;

typedef struct pixel_s{
    color_t color;
    char text[4];
    char init;
}pixel_t;

typedef struct screen_s{
    int height;
    int width;
    pixel_t ** pixel;
}screen_t;


screen_t screen_init(int height, int width);
void screen_show(screen_t screen);
void screen_draw_gpixel(screen_t *screen, v2i_t p,  color_t color);
void screen_draw_gline(screen_t *screen, v2i_t p1, v2i_t p2, color_t color);
void screen_draw_gtriangle(screen_t * screen, v2i_t p1, v2i_t p2, v2i_t p3, color_t color);

#endif