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
const double ANGLE_BETWEEN_POINTS = 0.1;

const double MAX_X2 = 1300;
const double MAX_Y2 = 650;

const rgb_color_t COLOR_INIT = {0, 0, 0};
const rgb_color_t INNER_COLOR = {1, 1, 1};
const vector_t BOX_SPEC = {150, 90};
const vector_t BOX_POINT = {MAX_X2 - 150 * 1.15, MAX_Y2 - 90 * 1.15};
const double SPACING_BOXES = 12;
const double SPACING_BOX_GAP = (MAX_Y2 - 50 - (4 * 90))/4;
const double SPACING = 6;
const vector_t BOARD_SPEC = {900, 600};
const vector_t BOARD_POINT = {100, MAX_Y2/2};
const vector_t SEGUEWAY_LEFT_SPEC = {43, 20};
const double SEGUEWAY_DELTA = 48;
const vector_t CORNER_SPEC = {75, 70};
const double CORNER_DELTA = 4;
const double ROOF_WIDTH = 660;
const vector_t CONE_SPEC = {210, 260};
const vector_t CONE_POINT = {500, 35};
const vector_t ALLEY_SPEC = {100, 250};
const vector_t ALLEY_POINT = {900 - 3 + 50/2, MAX_Y2/2 - 600/2 + 250/2};
const vector_t WALL_HEIGHT = {245, 260};
const vector_t LEFT_WALL_SPEC = {168, 413};
const vector_t LOSING_SPEC = {250, 20};

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

list_t *circle_sector(size_t rad, double min_rad, double max_rad) {
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
    return points;
}


void scene_setup(scene_t *scene){
    // Sets up 4 boxes
    for (int i = 0; i < 4; i++){
        body_t *box = make_box(BOX_SPEC.x, BOX_SPEC.y, COLOR_INIT, 2);
        body_t *inner_box = make_box(BOX_SPEC.x - SPACING_BOXES, BOX_SPEC.y - SPACING_BOXES, INNER_COLOR, 2);
        body_set_centroid(box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        body_set_centroid(inner_box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        scene_add_body(scene, box);
        scene_add_body(scene, inner_box);
    }

    // Sets up pinball border
    body_t *border1 = make_box(SPACING, BOARD_SPEC.y, COLOR_INIT, 1);
    body_t *border2 = make_box(BOARD_SPEC.x, SPACING, COLOR_INIT, 1);
    body_t *border3 = make_box(SPACING, BOARD_SPEC.y, COLOR_INIT, 1);
    body_t *border4 = make_box(BOARD_SPEC.x, SPACING, COLOR_INIT, 1);
    body_set_centroid(border1, BOARD_POINT);
    body_set_centroid(border2, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2, BOARD_POINT.y + BOARD_SPEC.y/2});
    body_set_centroid(border3, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x, BOARD_POINT.y});
    body_set_centroid(border4, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2, BOARD_POINT.y - BOARD_SPEC.y/2});
    scene_add_body(scene, border1);
    scene_add_body(scene, border2);
    scene_add_body(scene, border3);
    scene_add_body(scene, border4);

    // Sets up ball alley
    body_t *alley1 = make_box(SPACING, ALLEY_SPEC.y, COLOR_INIT, 1);
    body_t *alley2 = make_box(ALLEY_SPEC.x, SPACING, COLOR_INIT, 1);
    body_t *alley3 = make_box(SPACING, ALLEY_SPEC.y, COLOR_INIT, 1);
    body_set_centroid(alley1, (vector_t) {ALLEY_POINT.x - ALLEY_SPEC.x/2, ALLEY_POINT.y});
    body_set_centroid(alley2, (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y - ALLEY_SPEC.y/2});
    body_set_centroid(alley3, (vector_t) {ALLEY_POINT.x + ALLEY_SPEC.x/2, ALLEY_POINT.y});
    scene_add_body(scene, alley1);
    scene_add_body(scene, alley2);
    scene_add_body(scene, alley3);

    double alley_top = ALLEY_POINT.y + ALLEY_SPEC.y/2;

    // Sets up segueway
    body_t *segueway = make_trapezoid(SEGUEWAY_LEFT_SPEC.x, SEGUEWAY_LEFT_SPEC.y, SPACING, -1, COLOR_INIT, 1);
    body_set_centroid(segueway, (vector_t) {ALLEY_POINT.x - SEGUEWAY_DELTA, alley_top});
    scene_add_body(scene, segueway);

    // Sets up outer walls
    body_t *wall_left = make_box(SPACING, WALL_HEIGHT.x, COLOR_INIT, 1);
    body_t *wall_right = make_box(SPACING, WALL_HEIGHT.y, COLOR_INIT, 1);
    body_set_centroid(wall_left, LEFT_WALL_SPEC);
    body_set_centroid(wall_right, (vector_t) {ALLEY_POINT.x  + ALLEY_SPEC.x/2, alley_top + WALL_HEIGHT.y/2});
    scene_add_body(scene, wall_left);
    scene_add_body(scene, wall_right);

    // Sets up corner guards
    body_t *corner_left = make_trapezoid(CORNER_SPEC.x, CORNER_SPEC.y, SPACING, 1, COLOR_INIT, 1);
    body_t *corner_right = make_trapezoid(CORNER_SPEC.x, CORNER_SPEC.y, SPACING, -1, COLOR_INIT, 1);
    body_set_centroid(corner_left, (vector_t) {LEFT_WALL_SPEC.x - CORNER_DELTA, alley_top + WALL_HEIGHT.y});
    body_set_centroid(corner_right, (vector_t) {ALLEY_POINT.x  + ALLEY_SPEC.x/2 + CORNER_DELTA, alley_top + WALL_HEIGHT.y});
    scene_add_body(scene, corner_left);
    scene_add_body(scene, corner_right);

    // Sets up roof
    body_t *roof = make_box(ROOF_WIDTH, SPACING, COLOR_INIT, 1);
    body_set_centroid(roof, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2 + 3.2*SPACING, alley_top + WALL_HEIGHT.y + CORNER_SPEC.y - SPACING/2});
    scene_add_body(scene, roof);

    // Sets up cone bottom
    body_t *cone1 = make_trapezoid(CONE_SPEC.x, CONE_SPEC.y, SPACING, 1, COLOR_INIT, 1);
    body_t *cone2 = make_trapezoid(CONE_SPEC.x, CONE_SPEC.y, SPACING, -1, COLOR_INIT, 1);
    body_set_centroid(cone1, (vector_t) {CONE_POINT.x + LOSING_SPEC.x/2 - SPACING, CONE_POINT.y});
    body_set_centroid(cone2, (vector_t) {CONE_POINT.x - LOSING_SPEC.x/2 + SPACING, CONE_POINT.y});
    scene_add_body(scene, cone1);
    scene_add_body(scene, cone2);

    // Sets up losing area
    body_t *losing_area = make_box(LOSING_SPEC.x, LOSING_SPEC.y, COLOR_INIT, 2);
    body_set_centroid(losing_area, CONE_POINT);
    scene_add_body(scene, losing_area);
}
