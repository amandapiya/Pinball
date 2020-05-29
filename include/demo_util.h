#ifndef __DEMO_UTIL_H__
#define __DEMO_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "body.h"
#include "vector.h"
#include "list.h"

body_t *make_box(double width, double height, rgb_color_t color, int player);

body_t *make_trapezoid(double width, double height, double spacing, double slope, rgb_color_t color, int player);

list_t *circle_sector(size_t rad, double min_rad, double max_rad);

#endif // #ifndef __DEMO_UTIL_H__
