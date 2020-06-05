#include "demo_util.h"
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
const double EPSILON = 0.001;
const double ANGLE_BETWEEN_POINTS = 0.3;

body_t *make_box(double width, double height, rgb_color_t color, int spring){
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

    body_aux_t *aux = malloc(sizeof(body_aux_t));
    double mass;
    if (spring == 1){
        mass = 200.0;
        *aux = (body_aux_t){false, true, false};
    }else{
        *aux = (body_aux_t) {false, false, false};
        mass = INFINITY;
    }
    return body_init_with_info(rect, mass, color, aux, free);
}

body_t *make_trapezoid(double width, double height, double spacing, double slope, rgb_color_t color, int player){
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {0, 0};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {slope * width, height};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {slope * (width + spacing), height - spacing};
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = (vector_t) {slope * spacing, -1 * spacing};
    list_add(rect, v);

    double mass = INFINITY;
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    *aux = (body_aux_t){false, false, false};
    return body_init_with_info(rect, mass, color, aux, free);
}

body_t *make_circle(size_t rad, double min_rad, double max_rad,
  rgb_color_t color, double mass, int player) {
    vector_t center = VEC_ZERO;
    list_t *points = list_init(1, free);
    double theta = min_rad;
    if (fabs(min_rad - 0) > EPSILON || fabs(max_rad - 2 * M_PI) > EPSILON) {
        vector_t *c = malloc(sizeof(vector_t));
        *c = center;
        list_add(points, c);
    }
    while (theta < max_rad) {
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta) * rad;
        vertex_add->y = sin(theta) * rad;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(points, point);
        theta += ANGLE_BETWEEN_POINTS;
        free(vertex_add);
    }
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    if (player == 1){
        *aux = (body_aux_t){true, false, false};
    }else{
        *aux = (body_aux_t) {false, false, false};
    }
    return body_init_with_info(points, mass, color, aux, free);
}

