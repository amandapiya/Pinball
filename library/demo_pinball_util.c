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

const double MASS = 10;
const double MAX_RAD = 2 * M_PI;
const double BETWEEN_POINTS = 0.05;
const int INIT_HEALTH = 2;
const double TRAP_WIDTH = 250;
const double TRAP_HEIGHT = 100;
const double TRAP_SCALE = 1.2;

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
    double mass = MASS;
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    if (player == 1){
      *aux = (body_aux_t){false, true, false};
      mass = INFINITY;
    }else{
      *aux = (body_aux_t) {true, true, true};
    }
    return body_init_with_info(rect, mass, color, aux, free);
}

body_t *make_trapezoid(double scale, rgb_color_t color, int player){
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));

    v = malloc(sizeof(*v));
    *v = (vector_t) {(TRAP_WIDTH + scale) / 3, scale};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {scale + TRAP_SCALE * scale, TRAP_HEIGHT - scale};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {TRAP_WIDTH - 2 * TRAP_SCALE * scale , TRAP_HEIGHT - scale};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {(TRAP_WIDTH - scale) * 2/3, scale};
    list_add(rect, v);


    double mass = MASS;
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    if (player == 1){
      *aux = (body_aux_t){false, true, false};
      mass = INFINITY;
    }else{
      *aux = (body_aux_t) {true, true, true};
    }
    return body_init_with_info(rect, mass, color, aux, free);
}
