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
#include "swinger.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 650

const double MIN_XY = 0;
const double MAX_X = 1300;
const double MAX_Y = 650;
const double TIME_DIVISION = 11;

//void swinger_tick(swinger_t s1, swinger_t s2){


int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    double total_time = 0.0;

    swinger_t *s = swinger_init((vector_t){300, 300}, M_PI/2, 100, (rgb_color_t){1.0, 0, 0});
    body_t *star = make_star(6, 100, (vector_t){500, 500},(rgb_color_t){1.0, 0, 1.0});

    while (!sdl_is_done()){
        double dt = time_since_last_tick();
        total_time += dt;
        sdl_clear();
        //scene_tick(scene, dt);
        //sdl_render_scene(scene);
        sdl_draw_polygon(body_get_shape(star), (rgb_color_t){1.0, 1.0, 0});
        sdl_draw_polygon(swinger_get_shape(s), (rgb_color_t){1.0, 0, 0});
        sdl_show();
    }
    swinger_free(s);
    body_free(star);
    scene_free(scene);
  }
