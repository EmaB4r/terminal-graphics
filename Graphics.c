#include "Graphics.h"

int round_to_int(float f){
    int i=(int) f;
    f=f-i;
    return (f>=0.5)? i+1 : i;
}

screen_t screen_init(int height, int width){
    screen_t s;
    s.height=height;
    s.width=width;
    s.pixel=calloc(height, sizeof(pixel_t*));
    for(int i=0; i<height; i++){
        s.pixel[i]=calloc(width, sizeof(pixel_t));
    }
    return s;
}

void screen_draw_gpixel(screen_t *screen, int x, int y, color_t color){
    screen->pixel[y][x].text[0]=screen->pixel[y][x].text[1]=' ';
    screen->pixel[y][x].color=color;
    screen->pixel[y][x].init=1;
}

void screen_show(screen_t screen){
    for(int y=0; y<screen.height; y++){
        for(int x=0; x<screen.width; x++){
            if(screen.pixel[y][x].init) terminal_set_rgb_background(
                screen.pixel[y][x].color.r,
                screen.pixel[y][x].color.g,
                screen.pixel[y][x].color.b
            );
            fputs("  ", stdout);
            terminal_reset_color();
        }
        putc('\n', stdout);
    }
    terminal_cursor_left(screen.width);
    terminal_cursor_up(screen.height);
}

/*
void screen_set_pixel(screen_t screen, int x, int y, color_t color){
    screen.pixel[y][x].x=x;
    screen.pixel[y][x].y=y;
    screen.pixel[y][x].color=color;
}

void clear_screen(){
    clear_terminal();
    set_terminal_cursor(0,0);
}

void draw_pixel(pixel_t pixel){
    set_terminal_cursor(pixel.y+1, pixel.x*2+1);
    terminal_set_rgb_background(pixel.color.r, pixel.color.g,  pixel.color.b);
    printf("  ");
    terminal_reset_color();
}

void draw_line(int x1, int y1, int x2, int y2, color_t color){
    pixel_t pixel;
    pixel.color=color;
    if(x1==x2 && y1==y2){
        pixel.x=x1;
        pixel.y=y1;
        draw_pixel(pixel);
        return;
    }
    if(x1==x2){
        pixel.x=x1;
        if(y1>y2)
            y1^=y2^=y1^=y2;
        for(int y=y1; y<=y2; y++){
            pixel.y=y;
            draw_pixel(pixel);
        }
        return;
    }
    if(x1>x2){
        x1^=x2^=x1^=x2;
        y1^=y2^=y1^=y2;
    }
    double m = (double)(y2-y1)/(x2-x1);
    double q = (double)(y2-m*x2);
    for(int x=x1, y; x<=x2; x++){
        y=round_to_int(m*x+q);
        pixel.y=round_to_int(m*x+q);
        pixel.x=x;
        draw_pixel(pixel);
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3){
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x1, y1, x3, y3);
}

void draw_image_row(int x, int y, char*row){
    set_terminal_cursor(y, x);
    printf("%s", row);
    fflush(stdout);
}

void draw_graph(float(*function)(float), color_t color){
    pixel_t pixel;
    color_t white={255, 255, 255};
    int graph_x_center=35;
    int graph_y_center=25;
    int old_x=-10;
    int old_y=-round_to_int(function(old_x))+graph_y_center;
    int y=0;
    draw_line(0, 25, 71, 25, white);
    draw_line(35, 0, 35, 35, white);
    for(int x=-9; x<=10; x++){
        y=-round_to_int(function(x))+graph_y_center;
        if(old_y>0 && y>0)
            draw_line(old_x+graph_x_center, old_y, x+graph_x_center, y, color);
        old_x=x;
        old_y=y;
    }
}*/