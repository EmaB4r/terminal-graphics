#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "ansi-terminal/Ansi.h"
#include <string.h>

typedef struct v2d{double x, y;}v2d_t;
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

#define GR_WHYTE (color_t){255, 255, 255}
#define GR_RED (color_t){255, 0, 0}
#define GR_GREEN (color_t){0, 255, 0}
#define GR_BLUE (color_t){0, 0, 255}
#define GR_BLACK (color_t){0, 0, 0}

#define SCREEN_LOOP(FPS, BODY) struct timeval TV;\
    long DELAYMS = 1000/FPS;\
    gettimeofday(&TV, NULL);\
    long PREVMS = TV.tv_sec * 1000 + TV.tv_usec / 1000;\
    while(1){\
    gettimeofday(&TV, NULL);\
    long NOWMS = TV.tv_sec * 1000 + TV.tv_usec / 1000;\
    if(NOWMS-PREVMS>DELAYMS){\
        gettimeofday(&TV, NULL);\
        PREVMS = TV.tv_sec * 1000 + TV.tv_usec / 1000;\
        BODY;\
    }\
}

screen_t screen_init(int height, int width);
void screen_show(screen_t screen);
void screen_draw_gpixel(screen_t *screen, v2i_t p,  color_t color);
void screen_draw_gline(screen_t *screen, v2i_t p1, v2i_t p2, color_t color);
void screen_draw_gtriangle(screen_t * screen, v2i_t p1, v2i_t p2, v2i_t p3, color_t color);
void screen_draw_gcircle(screen_t * screen, v2i_t center, int radius, color_t color);

#endif