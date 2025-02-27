#include "../Graphics.h"
#include <math.h>

typedef struct { float x, y, z; } v3d_t;

v3d_t cube_vertices[] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}
};

int edges[][2] = {
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7} 
};

#define NUM_VERTICES 8
#define NUM_EDGES 12
#define FOCAL_LENGTH 3.0f
#define SCALE_FACTOR 17.0f

v3d_t rotate_x(v3d_t p, float angle) {
    return (v3d_t){
        p.x,
        p.y * cos(angle) - p.z * sin(angle),
        p.y * sin(angle) + p.z * cos(angle)
    };
}

v3d_t rotate_y(v3d_t p, float angle) {
    return (v3d_t){
        p.x * cos(angle) + p.z * sin(angle),
        p.y,
        -p.x * sin(angle) + p.z * cos(angle)
    };
}

v2i_t project(v3d_t p, int width, int height) {
    float z = p.z + FOCAL_LENGTH;
    if (z < 0.1f) z = 0.1f; 
    
    int x = (int)((p.x * FOCAL_LENGTH / z) * SCALE_FACTOR) + width/2;
    int y = height/2 - (int)((p.y * FOCAL_LENGTH / z) * SCALE_FACTOR);
    return (v2i_t){x, y};
}

int main() {
    const int width = 220, height = 100;
    float theta = 0.0f, phi = 0.0f;
    
    canvas_t canvas = canvas_init(height, width);
    canvas_t bg = canvas_init(height, width);
    
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            canvas_draw_gpixel(&bg, (v2i_t){x,y}, GR_BLACK);

    SCREEN_LOOP(30, {
        canvas_copy(&canvas, &bg);
        
        // Update rotation angles
        theta += 0.04f;
        phi += 0.03f;
        
        // Project all vertices
        v2i_t projections[NUM_VERTICES];
        for(int i = 0; i < NUM_VERTICES; i++) {
            v3d_t rotated = rotate_x(cube_vertices[i], phi);
            rotated = rotate_y(rotated, theta);
            projections[i] = project(rotated, width, height);
        }
        
        // Draw all edges
        for(int i = 0; i < NUM_EDGES; i++) {
            v2i_t p1 = projections[edges[i][0]];
            v2i_t p2 = projections[edges[i][1]];
            canvas_draw_gline(&canvas, p1, p2, GR_WHYTE);
        }
        
        canvas_gshow(canvas);
        
        // Exit on 'q' press
        unsigned char key = getkey(stdin);
        if(key == 'q' || key == 'Q') break;
    });
    return 0;
}