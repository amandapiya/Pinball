#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "body_aux.h"
#include "forces.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "collision.h"
#include "body.h"
#include "swinger.h"
#include "key_handler_aux.h"
#include "demo_util.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 650

const double MIN_XY = 0;
const double MAX_X = 1300;
const double MAX_Y = 650;
const double TIME_DIVISION = 11;
const double MOMENTUM_CONSTANT = 10;

const rgb_color_t COLOR_INIT = {0, 0, 0};
const rgb_color_t INNER_COLOR = {1, 1, 1};
const vector_t BOX_SPEC = {150, 90};
const vector_t BOX_POINT = {MAX_X - 150 * 1.15, MAX_Y - 90 * 1.15};
const double SPACING_BOXES = 12;
const double SPACING_BOX_GAP = (MAX_Y - 50 - (4 * 90))/4;
const double SPACING = 6;
const vector_t BOARD_SPEC = {900, 600};
const vector_t BOARD_POINT = {100, MAX_Y/2};
const vector_t SEGUEWAY_LEFT_SPEC = {43, 20};
const double SEGUEWAY_DELTA = 48;
const vector_t CORNER_SPEC = {75, 70};
const double CORNER_DELTA = 4;
const double ROOF_WIDTH = 660;
const vector_t CONE_SPEC = {210, 260};
const vector_t CONE_POINT = {500, 35};
const vector_t ALLEY_SPEC = {100, 250};
const vector_t ALLEY_POINT = {900 - 3 + 50/2, MAX_Y/2 - 600/2 + 250/2};
const vector_t WALL_HEIGHT = {245, 260};
const vector_t LEFT_WALL_SPEC = {168, 413};
const vector_t LOSING_SPEC = {250, 20};

void make_score_template(scene_t *scene){
    // Sets up 4 boxes
    for (int i = 0; i < 4; i++){
        body_t *box = make_box(BOX_SPEC.x, BOX_SPEC.y, COLOR_INIT, 2);
        body_t *inner_box = make_box(BOX_SPEC.x - SPACING_BOXES, BOX_SPEC.y - SPACING_BOXES, INNER_COLOR, 2);
        body_set_centroid(box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        body_set_centroid(inner_box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        scene_add_body(scene, box);
        scene_add_body(scene, inner_box);
    }
}

void make_pinball_border(scene_t *scene){
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


void on_key(char key, key_event_type_t type, double held_time, void *key_handler_aux) {
    // we can delete this right? there is no "player"
    /*body_t *player = get_player(scene);
    if (player != NULL){
        if (type == KEY_PRESSED){
            switch (key) {
                    break;
            }
            body_tick(player, held_time / TIME_DIVISION);
        }
        else {
            body_set_velocity(player, VEC_ZERO);
        }
    }*/
    if (type == KEY_PRESSED){
        switch (key) {
            case (LEFT_ARROW):
                printf("LEFT\n");
                swinger_t *s = key_aux_get_swinger1(key_handler_aux);
                double momentum = held_time * MOMENTUM_CONSTANT;
                swinger_add_momentum(s, momentum);
                break;
            case (RIGHT_ARROW):
                printf("RIGHT\n");
                swinger_t *s2 = key_aux_get_swinger2(key_handler_aux);
                double momentum2 = -1 * held_time * MOMENTUM_CONSTANT;
                swinger_add_momentum(s2, momentum2);
                break;
        }
    }
}

void reset_game(scene_t *scene){
    // add ball
    rgb_color_t ball_color = (rgb_color_t) {0.50, 0.50, 0.50};
    double ball_mass = 5.0;
    double ball_error = 7;
    body_t *ball = make_circle(ALLEY_SPEC.x / 2 - ball_error, 0, 2 * M_PI, ball_color, ball_mass, 1);
    vector_t ball_start_pos = (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y + 10}    ;
    body_set_centroid(ball, ball_start_pos);
    scene_add_body(scene, ball);
}

int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    

    make_pinball_border(scene);
    make_score_template(scene);


    list_t *swingers = list_init(1, (free_func_t)swinger_free);
    // TODO: remove magic numbers
    swinger_t *s1 = swinger_init((vector_t){380, 150}, 11*M_PI/6, 110, (rgb_color_t){1.0, 0, 0});
    swinger_t *s2 = swinger_init((vector_t){620, 150}, 7*M_PI/6, 110, (rgb_color_t){1.0, 0, 0});
    list_add(swingers, s1);
    list_add(swingers, s2);
    double total_time = 0.0;

    reset_game(scene);
    sdl_on_key(on_key);

    while (!sdl_is_done()){
        double dt = time_since_last_tick();
  	total_time += dt;
	if (total_time > 1){
        //swinger_add_momentum(s1, 20);
        //swinger_add_momentum(s2, -20);
        //printf("ADDING MOMENTUM\n");
		total_time = 0;
    }
        //printf("TORQUE: %f\n", swinger_get_torque(s1));
        //printf("MOMENTUM: %f\n", swinger_get_momentum(s2));

        swinger_tick(s1, dt);
        swinger_tick(s2, dt);
        scene_tick(scene, dt);
        sdl_render_scene(scene, swingers);
    }
    swinger_free(s1);
    swinger_free(s2);
    scene_free(scene);
  }
