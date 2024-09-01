#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdlib.h>
#include "ansi-terminal/Ansi.h"
#include "String.h"
#include <string.h>


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
void screen_draw_gpixel(screen_t *screen, int x, int y, color_t color);
void screen_show(screen_t screen);

#endif