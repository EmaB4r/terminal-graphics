#include "../Graphics.h"
#include <math.h>

int main() {
    const int width = 220, height = 100;
    canvas_t canvas = canvas_init(height, width);
    
    // Ball physics parameters
    float pos_x = width / 2.0f, pos_y = height / 4.0f;
    float vel_x = 0.9f, vel_y = 0.0f;
    const float gravity = 0.15f;
    const float air_damping = 1;
    const float bounce_damping = 0.82f;
    const float ground_friction = 0.85f;
    
    // Size animation parameters
    float size_phase = 0.0f;
    const float size_speed = 0.12f;
    const float base_size = 5.0f;
    const float size_variation = 1.8f;
    
    color_t ball_color = GR_RED;
    color_t bg_color = GR_BLACK;

    // Initialize background
    canvas_t bg = canvas_init(height, width);
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
            canvas_draw_gpixel(&bg, (v2i_t){x,y}, bg_color);

    SCREEN_LOOP(30, {
        canvas_copy(&canvas, &bg);

        // Update physics
        vel_y += gravity;
        pos_x += vel_x;
        pos_y += vel_y;
        
        // Update size animation
        size_phase += size_speed;
        float current_radius = base_size + size_variation * sin(size_phase);
        
        // Wall collisions
        float effective_radius = current_radius + 0.5f; // Add padding
        
        // Left/Right collisions
        if(pos_x - effective_radius < 0) {
            pos_x = effective_radius;
            vel_x *= -bounce_damping;
        }
        else if(pos_x + effective_radius >= width) {
            pos_x = width - effective_radius - 1;
            vel_x *= -bounce_damping;
        }
        
        // Floor/ceiling collisions
        if(pos_y + effective_radius >= height) {
            pos_y = height - effective_radius - 1;
            vel_y *= -bounce_damping;
            vel_x *= ground_friction; // Ground friction
            vel_y += 0.2f; // Energy absorption
        }
        else if(pos_y - effective_radius < 0) {
            pos_y = effective_radius;
            vel_y *= -bounce_damping;
        }
        
        // Air resistance
        vel_x *= air_damping;
        vel_y *= air_damping;

        // Draw the ball with smooth position and size
        canvas_draw_gcircle(&canvas, 
            (v2i_t){(int)roundf(pos_x), (int)roundf(pos_y)}, 
            (int)roundf(current_radius), 
            ball_color
        );

        canvas_gshow(canvas);

        // Exit on Q press
        unsigned char key = getkey(stdin);
        if(key == 'q' || key == 'Q') break;
    });
    return 0;
}