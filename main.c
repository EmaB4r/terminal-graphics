#include "Graphics.h"

int f_to_int(double f){
    int i=(int) f;
    f=f-i;
    return (f>=0.5)? i+1 : i;
}

void update_ypos(v2d_t * pos, double * speed, double acc, long dt_ms){
    double dt_s = dt_ms/(double)1000;
    *speed= *speed + acc*dt_s;
    pos->y=pos->y + *speed * dt_s+ acc*dt_s*dt_s;
    //printf("%f, %lu, %f, %f\n", pos->y, dt_ms,dt_s, *speed);
}

int main(int argc, char const *argv[])
{
    char c=0;
    terminal_hide_cursor();
    v2d_t d_pos = {39, 39};
    v2i_t i_pos = {39, 39};
    double acc = 9.81;
    double speed = -20;
    screen_t sc = screen_init(90, 100);
    SCREEN_LOOP(120,{
        i_pos.y=f_to_int(d_pos.y);
        update_ypos(&d_pos, &speed, acc, DELAYMS);
        screen_draw_gcircle(&sc, i_pos, 3, GR_GREEN);
        screen_show(sc);
    })
    return 0;
}
