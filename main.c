#include "Graphics.h"
#define __USE_MISC
#include "unistd.h"
int main(int argc, char const *argv[])
{
    char c=0;
    terminal_hide_cursor();
    screen_t sc = screen_init(70, 70);
    while(1){
        screen_draw_gtriangle(&sc, (v2i_t){0,0},(v2i_t){69,69},(v2i_t){35,41}, (color_t){255, 255, 255});
        screen_show(sc);
        c++;
        c=c%256;
        usleep(5000);
    }
    return 0;
}
