#include "../Graphics.h"
#include <math.h>
#include <time.h>

#define MAP_SIZE 16
#define FOV 60
#define WALL_HEIGHT 15.0

int world_map[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,2,0,0,0,3,0,0,0,0,4,0,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,5,0,0,0,0,0,5,0,0,0,0,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,0,6,0,0,0,0,0,0,0,6,0,0,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,0,2,0,0,0,0,0,0,0,2,0,0,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,3,0,0,0,0,0,3,0,0,0,0,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,1},
    {1,0,0,4,0,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct {
    float x, y;
    float dir_x, dir_y;     // Direction vector
    float plane_x, plane_y; // Camera plane vector
} Player;

color_t wall_colors[] = {
    GR_BLACK,   // 0
    GR_RED,     // 1
    GR_GREEN,   // 2
    GR_BLUE,    // 3
    GR_WHYTE,   // 4
    {255,165,0},// 5 (Orange)
    {128,0,128},// 6 (Purple)
    {255,192,203}//7 (Pink)
};

void cast_rays(canvas_t *canvas, Player *p) {
    for(int x = 0; x < canvas->width; x++) {
        float camera_x = 2 * x / (float)canvas->width - 1;
        float ray_dir_x = p->dir_x + p->plane_x * camera_x;
        float ray_dir_y = p->dir_y + p->plane_y * camera_x;

        int map_x = (int)p->x;
        int map_y = (int)p->y;
        
        float delta_dist_x = fabs(1 / ray_dir_x);
        float delta_dist_y = fabs(1 / ray_dir_y);
        float side_dist_x, side_dist_y;
        int step_x, step_y;
        int hit = 0, side;
        
        if(ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = (p->x - map_x) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - p->x) * delta_dist_x;
        }
        
        if(ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = (p->y - map_y) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - p->y) * delta_dist_y;
        }
        
        while(!hit) {
            if(side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }
            if(map_x < 0 || map_y < 0 || map_x >= MAP_SIZE || map_y >= MAP_SIZE) break;
            if(world_map[map_x][map_y] > 0) hit = 1;
        }

        if(hit) {
            float perp_wall_dist = side == 0 ? 
                (map_x - p->x + (1 - step_x)/2) / ray_dir_x :
                (map_y - p->y + (1 - step_y)/2) / ray_dir_y;
                
            int line_height = (int)(canvas->height / perp_wall_dist);
            int draw_start = -line_height/2 + canvas->height/2;
            int draw_end = line_height/2 + canvas->height/2;

            color_t color = wall_colors[world_map[map_x][map_y]];
            if(side == 1) color = (color_t){color.r/2, color.g/2, color.b/2};

            canvas_draw_gline(canvas, (v2i_t){x, draw_start}, (v2i_t){x, draw_end}, color);
        }
    }
}

int main() {
    const int W = 220, H = 100;
    canvas_t canvas = canvas_init(H, W);
    Player player = {
        .x = 8.5, 
        .y = 8.5,
        .dir_x = 1.0,  // Initial direction vector (facing right)
        .dir_y = 0.0,
        .plane_x = 0.0,   // Initial camera plane (perpendicular to direction)
        .plane_y = 0.66   // Determines FOV
    };
    terminal_set_size(H, W);

    SCREEN_LOOP(30, {
        unsigned char key = getkey(stdin);
        float move_speed = 0.08;
        float rot_speed = 0.05;
        
        // Movement
        if(key == 'w') {
            player.x += player.dir_x * move_speed;
            player.y += player.dir_y * move_speed;
        }
        if(key == 's') {
            player.x -= player.dir_x * move_speed;
            player.y -= player.dir_y * move_speed;
        }
        
        // Rotation
        if(key == 'd') {
            // Rotate left (counter-clockwise)
            float cos_rot = cos(rot_speed);
            float sin_rot = sin(rot_speed);
            
            // Rotate direction vector
            float old_dir_x = player.dir_x;
            player.dir_x = old_dir_x * cos_rot - player.dir_y * sin_rot;
            player.dir_y = old_dir_x * sin_rot + player.dir_y * cos_rot;
            
            // Rotate plane vector
            float old_plane_x = player.plane_x;
            player.plane_x = old_plane_x * cos_rot - player.plane_y * sin_rot;
            player.plane_y = old_plane_x * sin_rot + player.plane_y * cos_rot;
        }
        if(key == 'a') {
            // Rotate right (clockwise)
            float cos_rot = cos(-rot_speed);
            float sin_rot = sin(-rot_speed);
            
            // Rotate direction vector
            float old_dir_x = player.dir_x;
            player.dir_x = old_dir_x * cos_rot - player.dir_y * sin_rot;
            player.dir_y = old_dir_x * sin_rot + player.dir_y * cos_rot;
            
            // Rotate plane vector
            float old_plane_x = player.plane_x;
            player.plane_x = old_plane_x * cos_rot - player.plane_y * sin_rot;
            player.plane_y = old_plane_x * sin_rot + player.plane_y * cos_rot;
        }
        if(key == 'q') break;
    
        // Clear canvas
        for(int y = 0; y < H; y++)
            for(int x = 0; x < W; x++)
                canvas_draw_gpixel(&canvas, (v2i_t){x,y}, 
                    y > H/2 ? (color_t){40,40,40} : (color_t){80,80,80});
    
        cast_rays(&canvas, &player);
        
        // Draw UI
        canvas_draw_gline(&canvas, (v2i_t){W/2-2, H/2}, (v2i_t){W/2+2, H/2}, GR_WHYTE);
        canvas_draw_gline(&canvas, (v2i_t){W/2, H/2-2}, (v2i_t){W/2, H/2+2}, GR_WHYTE);
    
        canvas_gshow(canvas);
    });

    return 0;
}