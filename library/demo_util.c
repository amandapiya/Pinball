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

const double MASS_INIT = 200;
const double MAX_RAD = 2 * M_PI;
const double BETWEEN_POINTS = 0.05;
const int INIT_HEALTH = 2;
const double TRAP_WIDTH = 250;
const double TRAP_HEIGHT = 100;
const double TRAP_SCALE = 1.2;
const double EPSILON = 0.001;
const double ANGLE_BETWEEN_POINTS = 0.01;
const double MASS = 200;
const double INNER_SIDE_RATIO = 1/2.5;

const body_aux_t PLAYER_AUX = {true, false, false, false, false};    //TFFFF
const body_aux_t SPRING_AUX = {false, true, false, false, false};    //FTFFF
const body_aux_t STAR_AUX = {false, false, true, false, false};   //FFTFF
const body_aux_t EARTH_AUX = {false, false, false, true, false};     //FFFTF
const body_aux_t BRIDGE_AUX = {false, false, false, false, true};  //FFFFT
const body_aux_t GENERIC_AUX = {false, false, false, false, false};  //F

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
      half_shape.y *= -1;
    }

    body_aux_t *aux = malloc(sizeof(body_aux_t));
    double mass = INFINITY;
    if (spring == 1){
        *aux = SPRING_AUX;
        mass = MASS_INIT;
    }else if (spring == 2){
        *aux = BRIDGE_AUX;
    }else{
        *aux = GENERIC_AUX;
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

    body_aux_t *aux = malloc(sizeof(body_aux_t));
    *aux = GENERIC_AUX;
    return body_init_with_info(rect, INFINITY, color, aux, free);
}

body_t *make_circle(size_t rad, double min_rad, double max_rad, rgb_color_t color, double mass, int player) {
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
        *aux = PLAYER_AUX;
     }
    else if (player == 2){
        *aux = EARTH_AUX;
    }
    else{
        *aux = GENERIC_AUX;
    }
    return body_init_with_info(points, mass, color, aux, free);
}

body_t *make_star(size_t num_arms, size_t rad, rgb_color_t color){
    vector_t center = VEC_ZERO;
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    *aux = STAR_AUX;
    list_t *vertices = list_init(1, free);
    double angle_between_vertices = M_PI/(num_arms);
    double theta = 0;
    for (int i = 0; i < num_arms; i++){
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta)*rad;
        vertex_add->y = sin(theta)*rad;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(vertices, point);
        theta += angle_between_vertices;
        vertex_add->x = cos(theta)*INNER_SIDE_RATIO*rad;
        vertex_add->y = sin(theta)*INNER_SIDE_RATIO*rad;
        vector_t *point2 = malloc(sizeof(vector_t));
        *point2 = vec_add(center, *vertex_add);
        list_add(vertices, point2);
        free(vertex_add);
        theta += angle_between_vertices;
    }
    return body_init_with_info(vertices, INFINITY, color, aux, free);
}

body_t *make_accelerator(double rad, double height, vector_t center, rgb_color_t color){
    body_aux_t *aux = malloc(sizeof(body_aux_t));
    *aux = GENERIC_AUX;
   	list_t *vertices = list_init(1, free);
    vector_t *v1 = malloc(sizeof(*v1));
    *v1 = (vector_t) {center.x, center.y};
    list_add(vertices, v1);
    vector_t *v2 = malloc(sizeof(*v2));
    *v2 = (vector_t) {center.x + rad, center.y - height};
    list_add(vertices, v2);
    vector_t *v3 = malloc(sizeof(*v3));
    *v3 = (vector_t) {center.x + rad, center.y};
    list_add(vertices, v3);
    vector_t *v4 = malloc(sizeof(*v4));
    *v4 = (vector_t) {center.x, center.y + height};
    list_add(vertices, v4);
    vector_t *v5 = malloc(sizeof(*v5));
    *v5 = (vector_t) {center.x - rad, center.y};
    list_add(vertices, v5);
    vector_t *v6 = malloc(sizeof(*v5));
    *v6 = (vector_t) {center.x - rad, center.y - height};
    list_add(vertices, v6);
    body_t *acc = body_init_with_info(vertices, 1, color, (void *)aux, free); // maybe change mass
    return acc;
}
