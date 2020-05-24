#ifndef __DEMO_PINBALL_UTIL_H__
#define __DEMO_PINBALL_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "body.h"
#include "vector.h"
#include "list.h"

body_t *make_box(double width, double height, rgb_color_t color, int player);

body_t *make_trapezoid(double scale, rgb_color_t color, int player);

#endif // #ifndef __DEMO_PINBALL_UTIL_H__
