#include "demo_pinball_util.h"

#include "vector.h"
#include "list.h"
#include "body.h"
#include "scene.h"
#include "color.h"
#include "polygon.h"
#include "body_aux.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const double RECT_MASS = INFINITY;
const double MAX_RAD = 2 * M_PI;
const double BETWEEN_POINTS = 0.05;
const int INIT_HEALTH = 2;

body_t *make_box(double width, double height, rgb_color_t color, int player){
    vector_t centroid = VEC_ZERO;
    vector_t half_shape = {width/2, height/2};
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    for (int i = 0; i < 2; i++){
      v = malloc(sizeof(*v));
      *v = vec_add(centroid, half_shape);
      list_add(rect, v);
      half_shape.x  *= -1;
      v = malloc(sizeof(*v));
      *v = vec_subtract(centroid, half_shape);
      list_add(rect, v);
      half_shape.y  *= -1;
    }
    double mass = RECT_MASS;
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    if (player == 0){
      *aux = (body_aux_t){true, false, false};
    }else if (player == 1){
      *aux = (body_aux_t){false, true, false};
      mass = INFINITY;
    }else{
      *aux = (body_aux_t) {true, true, true};
    }
    return body_init_with_info(rect, mass, color, aux, free);
}
