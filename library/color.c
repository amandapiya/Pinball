#include "color.h"
#include <math.h>

const double MULT = 0.5;
// const float ADD = 0.00723;

rgb_color_t lighter_color(rgb_color_t color){
    float new_r = color.r + MULT * (1 - color.r);
    float new_g = color.g + MULT * (1 - color.g);
    float new_b = color.b + MULT * (1 - color.b);
    rgb_color_t result = (rgb_color_t){new_r, new_g, new_b};
    return result;
}

rgb_color_t phase_color(rgb_color_t og_color, double time_passed){
    // phases colors through purple shades, START WITH BLUE VALUE at 1.0
    float new_r = og_color.r;
    float new_g = og_color.g;
    float new_b = og_color.b;
    if (new_r < 1){ // R = 0
        new_r = fabs(cos(time_passed));
    }
    else {
        new_r = 1 - fabs(cos(time_passed));
    }
    if (new_g < 1){ // R = 0
        new_g = fabs(cos(time_passed));
    }
    else {
        new_g = 1 - fabs(cos(time_passed));
    }
    if (new_b < 1){ // R = 0
        new_b = fabs(cos(time_passed));
    }
    else {
        new_b = 1 - fabs(cos(time_passed));
    }
    return (rgb_color_t){new_r, new_g, new_b};
}
