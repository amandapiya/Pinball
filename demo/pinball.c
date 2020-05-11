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
#include "demo_util.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 650

const double MIN_XY = 0;
const double MAX_X = 1300;
const double MAX_Y = 650;
const double TIME_DIVISION = 11;

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
                break;
            case (RIGHT_ARROW):
                printf("RIGHT\n");
                break;
        }
    }
}

void reset_game(scene_t *scene){
    //is it possible to just free these objects? not free scene setup
}

int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    list_t *swingers = list_init(1, (free_func_t)swinger_free);
    swinger_t *s1 = swinger_init((vector_t){380, 150}, 11*M_PI/6, 110, (rgb_color_t){1.0, 0, 0});
    swinger_t *s2 = swinger_init((vector_t){620, 150}, 7*M_PI/6, 110, (rgb_color_t){1.0, 0, 0});
    list_add(swingers, s1);
    list_add(swingers, s2);
    double total_time = 0.0;

    scene_setup(scene);
    reset_game(scene);
    sdl_on_key(player_on_key);

    while (!sdl_is_done()){
        double dt = time_since_last_tick();
  	total_time += dt;
	if (total_time > 1){
                swinger_add_momentum(s1, 20);
                swinger_add_momentum(s2, -20);
                printf("ADDING MOMENTUM\n");
		total_time = 0;
        }
        printf("TORQUE: %f\n", swinger_get_torque(s1));
        printf("MOMENTUM: %f\n", swinger_get_momentum(s2));

        swinger_tick(s1, dt);
        swinger_tick(s2, dt);
        scene_tick(scene, dt);
        sdl_render_scene(scene, swingers);
    }
    swinger_free(s1);
    swinger_free(s2);
    scene_free(scene);
  }
