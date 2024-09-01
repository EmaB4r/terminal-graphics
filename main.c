#include "Graphics.h"
#define __USE_MISC
#include "unistd.h"
int main(int argc, char const *argv[])
{
    char c=0;
    while(1){
        screen_t sc = screen_init(97, 97);
        for(int y=0; y<sc.height; y++){
            for(int x=0; x<sc.width; x++){
                screen_draw_gpixel(&sc, x, y, (color_t){.r=c, .g=c, .b=c});
            }
        }
        screen_show(sc);
        c++;
        c=c%256;
        usleep(1000);
    }
    return 0;
}
