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
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

const double MIN_XY = 0;
const double MAX_X = 800;
const double MAX_Y = 800;
const double TIME_DIVISION = 11;
const rgb_color_t BOX_COLOR = {0, 0, 0};
const rgb_color_t INNER_COLOR = {1, 1, 1};
const double RECT_WIDTH = 150;
const double RECT_HEIGHT = 50;
const vector_t BOX_INIT = {100, MAX_Y - RECT_HEIGHT*0.7};
const double SPACING = (MAX_X - 100 - (3 * RECT_WIDTH))/1.8 + RECT_WIDTH;
const double LINE = 12;

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
    for (int i = 0; i < 3; i++){
        body_aux_t *aux1 = malloc(sizeof(body_aux_t));
        body_aux_t *aux2 = malloc(sizeof(body_aux_t));
        *aux1 = (body_aux_t){false, false, false, -1};
        *aux2 = (body_aux_t){false, false, false, -1};
        body_t *box = make_box(RECT_WIDTH, RECT_HEIGHT, BOX_COLOR, 2);
        body_t *inner_box = make_box(RECT_WIDTH - LINE, RECT_HEIGHT - LINE, INNER_COLOR, 2);
        body_set_centroid(box, (vector_t) {BOX_INIT.x + (SPACING * i), BOX_INIT.y});
        body_set_centroid(inner_box, (vector_t) {BOX_INIT.x + (SPACING * i), BOX_INIT.y});
        body_set_info(box, aux1);
        body_set_info(inner_box, aux2);
        scene_add_body(scene, box);
        scene_add_body(scene, inner_box);
    }
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
