#include "color.h"

const double MULT = 0.5;
const float ADD = 0.01;

rgb_color_t lighter_color(rgb_color_t color){
    float new_r = color.r + MULT * (1 - color.r);
    float new_g = color.g + MULT * (1 - color.g);
    float new_b = color.b + MULT * (1 - color.b);
    rgb_color_t result = (rgb_color_t){new_r, new_g, new_b};
    return result;
}

rgb_color_t phase_color(rgb_color_t color){
    float new_r = color.r + ADD;
    float new_g = color.g + ADD;
    float new_b = color.b + ADD;
    if (new_r > 1){
        new_r -= 1;
    }
    if (new_g > 1){
        new_g -= 1;
    }
    if (new_b > 1){
        new_b -= 1;
    }
    return (rgb_color_t){new_r, new_g, new_b};
}