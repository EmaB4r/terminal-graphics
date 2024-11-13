#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include "ansi-terminal/Ansi.h"
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


typedef struct v2i{int x, y;}v2i_t;
typedef struct color_s{unsigned char r; unsigned char g; unsigned char b;}color_t;

//used to represent a single pixel on the canvas
typedef struct pixel_s{
    color_t bg_color;
    color_t px_color;
    char c;
}pixel_t;

//canvas handler
typedef struct canvas_s{
    int height;
    int width;
    pixel_t ** pixel;
}canvas_t;

//pixels positions grow from top to bottom and left to right
/*  0  1  2  3  4  5  6 - x
 0
 1
 2
 3
 4
 5
 6
 |
 y
*/


#define GR_WHYTE (color_t){255, 255, 255}
#define GR_RED (color_t){255, 0, 0}
#define GR_GREEN (color_t){0, 255, 0}
#define GR_BLUE (color_t){0, 0, 255}
#define GR_BLACK (color_t){0, 0, 0}

//loops a body like {int a = 5; printf("%d", a);} for FPS times a second
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

//sort of non-blockin getc(), returns 0 if there are no inputs from the stream
unsigned char getkey(FILE* stream);

//funtion that creates a height x width canvas handler and returns it
canvas_t canvas_init(int height, int width);

//copies all pixels from src to dest
void canvas_copy(canvas_t * dest, canvas_t * src);

//draws a graphical pixel onto the canvas at the given position
void canvas_draw_gpixel(canvas_t *canvas, v2i_t p,  color_t color);

//draws on the canvas a graphical line between the two positions 
void canvas_draw_gline(canvas_t *canvas, v2i_t p1, v2i_t p2, color_t color);

//draws a triangle on the canvas given three vertices
void canvas_draw_gtriangle(canvas_t * canvas, v2i_t p1, v2i_t p2, v2i_t p3, color_t color);

//draws a rectangle on the canvas given two vertices
void canvas_draw_grectangle(canvas_t *canvas, v2i_t p1, v2i_t p2, color_t bg_color, char fill);

//draws a circle on the canvas given teh center and radius
void canvas_draw_gcircle(canvas_t * canvas, v2i_t center, int radius, color_t color);

//displays the canvas on the terminal
void canvas_gshow(canvas_t canvas);

void canvas_draw_apixel(canvas_t *canvas, v2i_t p,  char c ,color_t px_color, color_t bg_color);
void canvas_draw_aline(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color);
void canvas_draw_atriangle(canvas_t * canvas, v2i_t p1, v2i_t p2, v2i_t p3, color_t px_color,color_t bg_color);
void canvas_draw_arectangle(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color, char fill);
void canvas_draw_acircle(canvas_t * canvas, v2i_t center, int radius, char c ,color_t px_color, color_t bg_color);
void canvas_ashow(canvas_t canvas);

#endif