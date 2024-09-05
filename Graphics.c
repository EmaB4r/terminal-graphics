#include "Graphics.h"


int round_to_int(float f){
    int i=(int) f;
    f=f-i;
    return (f>=0.5)? i+1 : i;
}

int square(int x) {return x*x;}

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
    if(p.x < 0 || p.y < 0 || p.x > screen->width-1 || p.y > screen->height-1) return;
    screen->pixel[p.y][p.x].text[0]=screen->pixel[p.y][p.x].text[1]=' ';
    screen->pixel[p.y][p.x].color=color;
    screen->pixel[p.y][p.x].init=1;
}

void screen_show(screen_t screen){
    for(int y=0; y<screen.height; y++){
        for(int x=0; x<screen.width; x++){
            if(screen.pixel[y][x].init) terminal_set_rgb_background(
                screen.pixel[y][x].color.r,
                screen.pixel[y][x].color.g,
                screen.pixel[y][x].color.b
            );
            screen.pixel[y][x].init=0;
            fputs("  ", stdout);
            terminal_reset_color();
        }
        putc('\n', stdout);
    }
    terminal_cursor_left(screen.width);
    terminal_cursor_up(screen.height);
}


void screen_draw_gline(screen_t *screen, v2i_t p1, v2i_t p2, color_t color){
    if(p1.x==p2.x && p1.y==p2.y){
        screen_draw_gpixel(screen, p1, color);
        return;
    }
    if(p1.x==p2.x){
        if(p1.y>p2.y)
            p1.y^=p2.y^=p1.y^=p2.y;
        for(int y=p1.y; y<=p2.y; y++){
            screen_draw_gpixel(screen, (v2i_t){p1.x, y}, color);
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
        screen_draw_gpixel(screen, (v2i_t){x,y}, color);
    }
}

void screen_draw_gtriangle(screen_t * screen, v2i_t p1, v2i_t p2, v2i_t p3, color_t color){
    screen_draw_gline(screen, p1, p2, color);
    screen_draw_gline(screen, p2, p3, color);
    screen_draw_gline(screen, p3, p1, color);
}

void screen_draw_gcircle(screen_t * screen, v2i_t center, int radius, color_t color){
    v2i_t corner1={center.x-radius, center.y-radius};
    v2i_t corner2={center.x+radius, center.y+radius};
    for(int y = corner1.y; y<=corner2.y; ++y){
        for(int x = corner1.x; x<=corner2.x; ++x){
            if(square(x-center.x) + square(y-center.y) <= square(radius)){
                screen_draw_gpixel(screen, (v2i_t){x, y}, color);
            }
        }
    } 
}
