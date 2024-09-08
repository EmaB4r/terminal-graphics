#include "Graphics.h"
#include "ansi-terminal/Ansi.h"
#include <stdio.h>

enum gtype{graphics, ascii};

int round_to_int(float f){
    int i=(int) f;
    f=f-i;
    return (f>=0.5)? i+1 : i;
}

int square(int x) {return x*x;}

void _draw_pixel(screen_t *screen, v2i_t p,  char c ,color_t px_color, color_t bg_color, enum gtype type){
    if(p.x < 0 || p.y < 0 || p.x > screen->width-1 || p.y > screen->height-1) return;
    screen->pixel[p.y][p.x].c=c;
    screen->pixel[p.y][p.x].px_color=px_color;
    screen->pixel[p.y][p.x].bg_color=bg_color;
    screen->pixel[p.y][p.x].init=1;
}

void _draw_line(screen_t *screen, v2i_t p1, v2i_t p2, char c ,color_t px_color, color_t bg_color, enum gtype type){
    if(p1.x==p2.x && p1.y==p2.y){
        _draw_pixel(screen, p1, c, px_color, bg_color, type);
        return;
    }
    if(p1.x==p2.x){
        if(p1.y>p2.y)
            p1.y^=p2.y^=p1.y^=p2.y;
        for(int y=p1.y; y<=p2.y; y++){
            _draw_pixel(screen, (v2i_t){p1.x, y}, c, px_color, bg_color, type);
        }
        return;
    }
    if(p1.x>p2.x){
        p1.x^=p2.x^=p1.x^=p2.x;
        p1.y^=p2.y^=p1.y^=p2.y;
    }
    double m = (double)(p2.y-p1.y)/(p2.x-p1.x);
    double q = (double)(p2.y-m*p2.x);
    for(int x=p1.x, y; x<=p2.x; x++){
        y=round_to_int(m*x+q);
        _draw_pixel(screen, (v2i_t){x,y}, c, px_color, bg_color, type);
    }
}

void _draw_triangle(screen_t *screen, v2i_t p1, v2i_t p2, v2i_t p3,char c ,color_t px_color, color_t bg_color, enum gtype type){
    _draw_line(screen, p1, p2, c ,px_color, bg_color, type);
    _draw_line(screen, p2, p3, c ,px_color, bg_color, type);
    _draw_line(screen, p3, p1, c ,px_color, bg_color, type);
}

void _draw_circle(screen_t * screen, v2i_t center, int radius, char c ,color_t px_color, color_t bg_color, enum gtype type){
    v2i_t corner1={center.x-radius, center.y-radius};
    v2i_t corner2={center.x+radius, center.y+radius};
    for(int y = corner1.y; y<=corner2.y; ++y){
        for(int x = corner1.x; x<=corner2.x; ++x){
            if(square(x-center.x) + square(y-center.y) <= square(radius)){
                _draw_pixel(screen, (v2i_t){x, y}, c, px_color, bg_color, type);
            }
        }
    }
}


screen_t screen_init(int height, int width){
    screen_t s;
    s.height=height;
    s.width=width;
    s.pixel=calloc(height, sizeof(pixel_t*));
    for(int i=0; i<height; i++){
        s.pixel[i]=calloc(width, sizeof(pixel_t));
    }
    terminal_hide_cursor();
    return s;
}

void screen_draw_gpixel(screen_t *screen, v2i_t p, color_t color){
    _draw_pixel(screen, p, ' ', color, color, graphics);
}

void screen_draw_apixel(screen_t *screen, v2i_t p,  char c ,color_t px_color, color_t bg_color){
    _draw_pixel(screen, p, c, px_color, bg_color, ascii);
}

void screen_draw_gline(screen_t *screen, v2i_t p1, v2i_t p2, color_t bg_color){
    _draw_line(screen, p1, p2, 'l', bg_color, bg_color, graphics);
}

void screen_draw_aline(screen_t *screen, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color){
    _draw_line(screen, p1, p2, c, px_color, bg_color, ascii);
}

void screen_draw_gtriangle(screen_t * screen, v2i_t p1, v2i_t p2, v2i_t p3, color_t bg_color){
    _draw_triangle(screen, p1, p2, p3, 't', bg_color, bg_color, graphics);
}

void screen_draw_atriangle(screen_t * screen, v2i_t p1, v2i_t p2, v2i_t p3, color_t px_color,color_t bg_color){
    _draw_triangle(screen, p1, p2, p3, 't', bg_color, bg_color, ascii);
}

void screen_draw_gcircle(screen_t * screen, v2i_t center, int radius, color_t bg_color){
    _draw_circle(screen,center, radius, 'c', bg_color, bg_color, graphics);
}

void screen_draw_acircle(screen_t * screen, v2i_t center, int radius, char c ,color_t px_color, color_t bg_color){;
    _draw_circle(screen,center, radius, c, px_color, bg_color, ascii);
}

void _show(screen_t screen, enum gtype type){
    for(int y=0; y<screen.height; y++){
        for(int x=0; x<screen.width; x++){
            if(type == ascii){
                if(screen.pixel[y][x].init) {
                    terminal_set_rgb_background(
                    screen.pixel[y][x].bg_color.r,
                    screen.pixel[y][x].bg_color.g,
                    screen.pixel[y][x].bg_color.b);
                    terminal_set_rgb_text(
                    screen.pixel[y][x].px_color.r,
                    screen.pixel[y][x].px_color.g,
                    screen.pixel[y][x].px_color.b);
                    
                    screen.pixel[y][x].init=0;
                    putc(screen.pixel[y][x].c, stdout);
                    terminal_reset_color();
                }
                else {
                    putc(' ', stdout);
                }
            }
            else{
                if(screen.pixel[y][x].init) {
                    terminal_set_rgb_background(
                    screen.pixel[y][x].bg_color.r,
                    screen.pixel[y][x].bg_color.g,
                    screen.pixel[y][x].bg_color.b);
                }
                screen.pixel[y][x].init=0;
                fputs("  ", stdout);
                terminal_reset_color();
            }
        }
        putc('\n', stdout);
    }
    terminal_cursor_left(screen.width);
    terminal_cursor_up(screen.height);
}


void screen_gshow(screen_t screen){
    _show(screen, graphics);
}

void screen_ashow(screen_t screen){
    _show(screen, ascii);
}