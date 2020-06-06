#ifndef __DEMO_UTIL_H__
#define __DEMO_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "body.h"
#include "vector.h"
#include "list.h"
#include "scene.h"

body_t *make_box(double width, double height, rgb_color_t color, int spring);

body_t *make_trapezoid(double width, double height, double spacing, double slope, rgb_color_t color, int player);

body_t *make_circle(size_t rad, double min_rad, double max_rad, rgb_color_t color, double mass, int player);

body_t *make_star(size_t num_arms, size_t rad, vector_t center_V, rgb_color_t color);

body_t *make_accelerator(double rad, double height, vector_t center, rgb_color_t color);

#endif // #ifndef __DEMO_UTIL_H__
