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
#include "demo_pinball_util.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 650

const double MIN_XY = 0;
const double MAX_X = 1300;
const double MAX_Y = 650;
const double TIME_DIVISION = 11;
const rgb_color_t BOX_COLOR = {0, 0, 0};
const rgb_color_t INNER_COLOR = {1, 1, 1};
const double RECT_WIDTH = 150;
const double RECT_HEIGHT = 90;
const vector_t BOX_INIT = {MAX_X - RECT_WIDTH * 1.15, MAX_Y - RECT_HEIGHT * 1.15};

const double SPACING = (MAX_Y - 50 - (4 * RECT_HEIGHT))/4;
const double SPACING2 = 6;
const double SPACING3 = 12;

const double BOARD_WIDTH = 900;
const double BOARD_HEIGHT = 600;
const double BOARD_INIT_X = 100;
const double BOARD_INIT_Y = MAX_Y/2;

const double CONE_SCALE = 5;
const double CONE_WIDTH = 100;
const double CONE_HEIGHT = 100;
const vector_t CONE_POINT = {207, 80};

const double ALLEY_WIDTH = 50;
const double ALLEY_HEIGHT = 258;
const vector_t ALLEY_POINT = {535, 200};


const vector_t LOSING_POINT = {330, 70};

body_t *get_player(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *player = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(player))->is_player) {
            return player;
        }
    }
    return NULL;
}

void player_on_key(char key, key_event_type_t type, double held_time, void *scene) {
    body_t *player = get_player(scene);
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
    }
}

void scene_setup(scene_t *scene){
    // Sets up 4 boxes
    for (int i = 0; i < 4; i++){
        body_t *box = make_box(RECT_WIDTH, RECT_HEIGHT, BOX_COLOR, 2);
        body_t *inner_box = make_box(RECT_WIDTH - SPACING3, RECT_HEIGHT - SPACING3, INNER_COLOR, 2);
        body_set_centroid(box, (vector_t) {BOX_INIT.x, BOX_INIT.y - i * (SPACING + RECT_HEIGHT)});
        body_set_centroid(inner_box, (vector_t) {BOX_INIT.x, BOX_INIT.y - i * (SPACING + RECT_HEIGHT)});
        scene_add_body(scene, box);
        scene_add_body(scene, inner_box);
    }

    // Sets up pinball border
    body_t *border1 = make_box(SPACING2, BOARD_HEIGHT, BOX_COLOR, 1);
    body_t *border2 = make_box(BOARD_WIDTH, SPACING2, BOX_COLOR, 1);
    body_t *border3 = make_box(SPACING2, BOARD_HEIGHT, BOX_COLOR, 1);
    body_t *border4 = make_box(BOARD_WIDTH, SPACING2, BOX_COLOR, 1);
    body_set_centroid(border1, (vector_t) {BOARD_INIT_X, BOARD_INIT_Y});
    body_set_centroid(border2, (vector_t) {BOARD_INIT_X + BOARD_WIDTH/2, BOARD_INIT_Y + BOARD_HEIGHT/2});
    body_set_centroid(border3, (vector_t) {BOARD_INIT_X + BOARD_WIDTH, BOARD_INIT_Y});
    body_set_centroid(border4, (vector_t) {BOARD_INIT_X + BOARD_WIDTH/2, BOARD_INIT_Y - BOARD_HEIGHT/2});
    scene_add_body(scene, border1);
    scene_add_body(scene, border2);
    scene_add_body(scene, border3);
    scene_add_body(scene, border4);

    // Sets up ball alley
    body_t *alley = make_box(ALLEY_WIDTH, ALLEY_HEIGHT, (rgb_color_t) {1, 0, 1}, 1);
    body_t *alley_inside = make_box(ALLEY_WIDTH - SPACING3, ALLEY_HEIGHT - SPACING3, INNER_COLOR, 2);
    body_set_centroid(alley, ALLEY_POINT);
    body_set_centroid(alley_inside, (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y + SPACING3});
    scene_add_body(scene, alley);
    scene_add_body(scene, alley_inside);

    // Sets up segueway


    // Sets up dome top


    // Sets up dome mid


    // Sets up dome bottom
    body_t *cone = make_trapezoid(0, BOX_COLOR, 1);
    body_t *cone_inside = make_trapezoid(CONE_SCALE, INNER_COLOR, 2);
    body_set_centroid(cone, CONE_POINT);
    body_set_centroid(cone_inside, (vector_t) {CONE_POINT.x, CONE_POINT.y + SPACING3});
    scene_add_body(scene, cone);
    scene_add_body(scene, cone_inside);


    // Sets up losing area
    body_t *losing_area = make_box(RECT_WIDTH, RECT_HEIGHT/2, BOX_COLOR, 1);
    body_set_centroid(losing_area, LOSING_POINT);
    scene_add_body(scene, losing_area);
}

void reset_game(scene_t *scene){


}

int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    double total_time = 0.0;

    scene_setup(scene);
    reset_game(scene);
    sdl_on_key(player_on_key);

    while (!sdl_is_done()){
        double dt = time_since_last_tick();
        total_time += dt;
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
  }
