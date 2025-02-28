#include "Graphics.h"
#include "ansi-terminal/Ansi.h"

// TODO: check if it would be better to buffer the current canvas 
// and the previous one and just print the differences instead of redrawing

#define square(x) x*x

enum gtype{graphics, ascii};

int round_to_int(float f){
    int i=(int) f;
    f=f-i;
    return (f>=0.5)? i+1 : i;
}

char terminal_initialized=0;

struct termios original_tio;

void save_terminal_state() {
    tcgetattr(STDIN_FILENO, &original_tio);
}

void restore_terminal_state() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
    terminal_show_cursor();
    terminal_set_cursor_pos(1, 1);
    clear_terminal();
    terminal_reset_color();
    exit(0);
}

void set_raw_mode() {
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

struct timeval timeout;
unsigned char getkey(FILE* stream){
    int fd = fileno(stream);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    int result = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (result > 0 && FD_ISSET(fd, &readfds)) return getc(stdin);
    return 0;
}


void _draw_pixel(canvas_t *canvas, v2i_t p,  char c ,color_t px_color, color_t bg_color, enum gtype type){
    if(p.x < 0 || p.y < 0 || p.x > canvas->width-1 || p.y > canvas->height-1) return;
    canvas->pixel[p.y][p.x].c=c;
    canvas->pixel[p.y][p.x].px_color=px_color;
    canvas->pixel[p.y][p.x].bg_color=bg_color;
}

void _draw_line(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color, enum gtype type) {
    int dx = abs(p2.x - p1.x);
    int dy = -abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx + dy;
    int current_x = p1.x;
    int current_y = p1.y;

    while(1) {
        _draw_pixel(canvas, (v2i_t){current_x, current_y}, c, px_color, bg_color, type);
        
        if(current_x == p2.x && current_y == p2.y) break;
        
        int e2 = 2 * err;
        if(e2 >= dy) { // Horizontal step
            if(current_x == p2.x) break;
            err += dy;
            current_x += sx;
        }
        if(e2 <= dx) { // Vertical step
            if(current_y == p2.y) break;
            err += dx;
            current_y += sy;
        }
    }
}

void _draw_triangle(canvas_t *canvas, v2i_t p1, v2i_t p2, v2i_t p3,char c ,color_t px_color, color_t bg_color, enum gtype type){
    _draw_line(canvas, p1, p2, c ,px_color, bg_color, type);
    _draw_line(canvas, p2, p3, c ,px_color, bg_color, type);
    _draw_line(canvas, p3, p1, c ,px_color, bg_color, type);
}

void _draw_circle(canvas_t * canvas, v2i_t center, int radius, char c ,color_t px_color, color_t bg_color, enum gtype type){
    v2i_t corner1={center.x-radius, center.y-radius};
    v2i_t corner2={center.x+radius, center.y+radius};
    
    for(int y = corner1.y; y<=corner2.y; ++y){
        for(int x = corner1.x; x<=corner2.x; ++x){
            int dx = (x - center.x);
            int dy = y - center.y;
            if(dx*dx + dy*dy <= radius*radius){
                _draw_pixel(canvas, (v2i_t){x, y}, c, px_color, bg_color, type);
            }
        }
    }
}

void _draw_rectangle(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color, enum gtype type, char fill){
    if(fill){
        if(p1.x>p2.x) p1.x^=p2.x^=p1.x^=p2.x;
        if(p1.y>p2.y) p1.x^=p2.x^=p1.x^=p2.x;
        for(int i = p1.y; i<= p2.y; i++){
            for(int j = p1.x; j<=p2.x; j++){
                canvas->pixel[i][j].c=c;
                canvas->pixel[i][j].px_color=px_color;
                canvas->pixel[i][j].bg_color=bg_color;
            }
        }
    }
    else{
        _draw_line(canvas, p1, (v2i_t){p2.x, p1.y}, c, px_color, bg_color, type);
        _draw_line(canvas, (v2i_t){p2.x, p1.y}, p2, c, px_color, bg_color, type);
        _draw_line(canvas, p2, (v2i_t){p1.x, p2.y}, c, px_color, bg_color, type);
        _draw_line(canvas, (v2i_t){p1.x, p2.y}, p1, c, px_color, bg_color, type);
    }
}


canvas_t canvas_init(int height, int width){
    if(!terminal_initialized){
        terminal_initialized=1;
        save_terminal_state();
        set_raw_mode();
        signal(SIGINT, restore_terminal_state);
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        clear_terminal();
    }
    canvas_t s;
    s.height=height;
    s.width=width;
    s.pixel=calloc(height, sizeof(pixel_t*));
    for(int i=0; i<height; i++){
        s.pixel[i]=calloc(width, sizeof(pixel_t));
    }
    terminal_hide_cursor();
    return s;
}

void canvas_copy(canvas_t * dest, canvas_t * src){
    if(dest->height==src->height && dest->width==src->width){
        for(int i = 0; i < src->height; ++i){
            for(int j = 0; j < src->width; ++j){
                dest->pixel[i][j] = src->pixel[i][j];
            }
        }
    }
}

void canvas_draw_gpixel(canvas_t *canvas, v2i_t p, color_t color){
    _draw_pixel(canvas, p, ' ', color, color, graphics);
}

void canvas_draw_apixel(canvas_t *canvas, v2i_t p,  char c ,color_t px_color, color_t bg_color){
    _draw_pixel(canvas, p, c, px_color, bg_color, ascii);
}

void canvas_draw_gline(canvas_t *canvas, v2i_t p1, v2i_t p2, color_t bg_color){
    _draw_line(canvas, p1, p2, 'l', bg_color, bg_color, graphics);
}

void canvas_draw_aline(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color){
    _draw_line(canvas, p1, p2, c, px_color, bg_color, ascii);
}

void canvas_draw_gtriangle(canvas_t * canvas, v2i_t p1, v2i_t p2, v2i_t p3, color_t bg_color){
    _draw_triangle(canvas, p1, p2, p3, 't', bg_color, bg_color, graphics);
}

void canvas_draw_atriangle(canvas_t * canvas, v2i_t p1, v2i_t p2, v2i_t p3, color_t px_color,color_t bg_color){
    _draw_triangle(canvas, p1, p2, p3, 't', px_color, bg_color, ascii);
}

void canvas_draw_gcircle(canvas_t * canvas, v2i_t center, int radius, color_t bg_color){
    _draw_circle(canvas,center, radius, 'c', bg_color, bg_color, graphics);
}

void canvas_draw_acircle(canvas_t * canvas, v2i_t center, int radius, char c ,color_t px_color, color_t bg_color){;
    _draw_circle(canvas,center, radius, c, px_color, bg_color, ascii);
}

void canvas_draw_arectangle(canvas_t *canvas, v2i_t p1, v2i_t p2, char c, color_t px_color, color_t bg_color, char fill){
    _draw_rectangle(canvas, p1, p2, c, px_color, bg_color, ascii, fill);
}
void canvas_draw_grectangle(canvas_t *canvas, v2i_t p1, v2i_t p2, color_t bg_color, char fill){
    _draw_rectangle(canvas, p1, p2, ' ' , bg_color, bg_color, graphics, fill);
}

void _show(canvas_t canvas, enum gtype type){
    for(int y=0; y<canvas.height; y++){
        for(int x=0; x<canvas.width; x++){
            if(type == ascii){
                if(canvas.pixel[y][x].c != 0) {
                    terminal_set_rgb_background(
                    canvas.pixel[y][x].bg_color.r,
                    canvas.pixel[y][x].bg_color.g,
                    canvas.pixel[y][x].bg_color.b);
                    terminal_set_rgb_text(
                    canvas.pixel[y][x].px_color.r,
                    canvas.pixel[y][x].px_color.g,
                    canvas.pixel[y][x].px_color.b);
                    
                    putc(canvas.pixel[y][x].c, stdout);
                }
                else {
                    putc(' ', stdout);
                }
            }
            else{
                terminal_set_rgb_background(
                canvas.pixel[y][x].bg_color.r,
                canvas.pixel[y][x].bg_color.g,
                canvas.pixel[y][x].bg_color.b);
                fputs("  ", stdout);
            }
            terminal_reset_color();

        }
        putc('\n', stdout);
    }
    terminal_cursor_left(canvas.width);
    terminal_cursor_up(canvas.height);
}


void canvas_gshow(canvas_t canvas){
    terminal_set_cursor_pos(2, 1);
    for(int y=0; y<canvas.height; y++){
        for(int x=0; x<canvas.width; x++){
            
            terminal_set_rgb_background(
            canvas.pixel[y][x].bg_color.r,
            canvas.pixel[y][x].bg_color.g,
            canvas.pixel[y][x].bg_color.b);
            fputs("  ", stdout);
            fflush(stdout);
        }
        terminal_reset_color();
        putc('\n', stdout);
    }
}

void canvas_ashow(canvas_t canvas){
    terminal_set_cursor_pos(2, 1);
    for(int y=0; y<canvas.height; y++){
        for(int x=0; x<canvas.width; x++){
            
            terminal_set_rgb_background(
            canvas.pixel[y][x].bg_color.r,
            canvas.pixel[y][x].bg_color.g,
            canvas.pixel[y][x].bg_color.b);
            terminal_set_rgb_text(
            canvas.pixel[y][x].px_color.r,
            canvas.pixel[y][x].px_color.g,
            canvas.pixel[y][x].px_color.b);
            
            putc(canvas.pixel[y][x].c, stdout);
            fflush(stdout);
        }
        putc('\n', stdout);
    }
}