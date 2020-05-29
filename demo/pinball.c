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
#include "demo_util.h"

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
const vector_t BOARD_INIT = {100, MAX_Y/2};

const double CONE_WIDTH = 150;
const double CONE_HEIGHT = 200;

const double ALLEY_WIDTH = 50;
const double ALLEY_HEIGHT = 250;
const vector_t ALLEY_POINT = {BOARD_WIDTH - 3, MAX_Y/2 - BOARD_HEIGHT/2 + ALLEY_HEIGHT/2};

const double SIDE_WALL_HEIGHT = 80;
const vector_t WALL = {247, 270};

const double LOSING_WIDTH = 250;
const double LOSING_HEIGHT = 20;
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
    body_set_centroid(border1, (vector_t) {BOARD_INIT.x, BOARD_INIT.y});
    body_set_centroid(border2, (vector_t) {BOARD_INIT.x + BOARD_WIDTH/2, BOARD_INIT.y + BOARD_HEIGHT/2});
    body_set_centroid(border3, (vector_t) {BOARD_INIT.x + BOARD_WIDTH, BOARD_INIT.y});
    body_set_centroid(border4, (vector_t) {BOARD_INIT.x + BOARD_WIDTH/2, BOARD_INIT.y - BOARD_HEIGHT/2});
    scene_add_body(scene, border1);
    scene_add_body(scene, border2);
    scene_add_body(scene, border3);
    scene_add_body(scene, border4);

    // Sets up ball alley
    body_t *alley1 = make_box(SPACING2, ALLEY_HEIGHT, (rgb_color_t) {1, 0, 1}, 1);
    body_t *alley2 = make_box(ALLEY_WIDTH, SPACING2, (rgb_color_t) {1, 0, 1}, 1);
    body_t *alley3 = make_box(SPACING2, ALLEY_HEIGHT, (rgb_color_t) {1, 0, 1}, 1);
    body_set_centroid(alley1, (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y});
    body_set_centroid(alley2, (vector_t) {ALLEY_POINT.x + ALLEY_WIDTH/2, ALLEY_POINT.y - ALLEY_HEIGHT/2});
    body_set_centroid(alley3, (vector_t) {ALLEY_POINT.x + ALLEY_WIDTH, ALLEY_POINT.y});
    scene_add_body(scene, alley1);
    scene_add_body(scene, alley2);
    scene_add_body(scene, alley3);

    // Sets up segueway


    // Sets up dome top
    body_aux_t *info = malloc(sizeof(body_aux_t));
    *info = (body_aux_t){true, true, true};
    body_t *dome = body_init_with_info(circle_sector(100, 0, 180), 10, BOX_COLOR, info, free);
    body_set_centroid(dome, (vector_t) {MAX_X/2, MAX_Y/2});
    //scene_add_body(scene, dome);

    // Sets up mid walls
    body_t *left_wall = make_box(SPACING2, SIDE_WALL_HEIGHT, BOX_COLOR, 1);
    body_t *right_wall = make_box(SPACING2, SIDE_WALL_HEIGHT, BOX_COLOR, 1);
    body_set_centroid(left_wall, (vector_t) {WALL.x, WALL.y});
    body_set_centroid(right_wall, (vector_t) {WALL.x + LOSING_WIDTH + 2 * CONE_WIDTH - SPACING2, WALL.y});
    scene_add_body(scene, left_wall);
    scene_add_body(scene, right_wall);

    // Sets up cone bottom
    double cone_x = BOARD_INIT.x + BOARD_WIDTH/2.15;
    double cone_y = BOARD_INIT.y - BOARD_HEIGHT/2 + LOSING_HEIGHT/2;
    body_t *cone1 = make_trapezoid(CONE_WIDTH, CONE_HEIGHT, SPACING2, 1, BOX_COLOR, 1);
    body_t *cone2 = make_trapezoid(CONE_WIDTH, CONE_HEIGHT, SPACING2, -1, BOX_COLOR, 1);
    body_set_centroid(cone1, (vector_t) {cone_x + LOSING_WIDTH/2 - SPACING2, cone_y});
    body_set_centroid(cone2, (vector_t) {cone_x - LOSING_WIDTH/2 + SPACING2, cone_y});
    scene_add_body(scene, cone1);
    scene_add_body(scene, cone2);

    // Sets up losing area
    body_t *losing_area = make_box(LOSING_WIDTH, LOSING_HEIGHT, BOX_COLOR, 2);
    body_set_centroid(losing_area, (vector_t) {cone_x, cone_y});
    scene_add_body(scene, losing_area);
}

void reset_game(scene_t *scene){
    //is it possible to just free these objects? not free scene setup

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
