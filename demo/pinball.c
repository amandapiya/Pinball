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
#include "color.h"
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
const double MID_X = 500;

//Scene constants
const rgb_color_t BLACK = {0, 0, 0};
const rgb_color_t INNER_COLOR = {1, 1, 1};
const vector_t BOX_SPEC = {200, 90};
const vector_t BOX_POINT = {MAX_X - 150 * 1.15, MAX_Y - 90 * 1.15};
const double SPACING_BOXES = 12;
const double SPACING_BOX_GAP = (MAX_Y - 50 - (4 * 90))/4;
const double SPACING = 6;
const vector_t BOARD_SPEC = {900, 600};
const vector_t BOARD_POINT = {100, MAX_Y/2};
const vector_t SEGUEWAY_LEFT_SPEC = {43, 20};
const double SEGUEWAY_DELTA = 48;
const double BRIDGE_WIDTH = 62;
const vector_t CORNER_SPEC = {75, 100};
const double CORNER_DELTA = 4;
const double ROOF_WIDTH = 650;
const vector_t CONE_SPEC = {210, 260};
const vector_t CONE_POINT = {500, 35};
const vector_t ALLEY_SPEC = {75, 477};
const vector_t ALLEY_POINT = {900 + 50/2, MAX_Y/2 - 600/2 + 480/2};
const vector_t WALL_HEIGHT = {215, 215};
const vector_t LEFT_WALL_POINT = {168, 396};
const vector_t RIGHT_WALL_POINT = {831, 396};
const vector_t LOSING_SPEC = {250, 20};
const rgb_color_t BALL_COLOR  = {1.0, 0.0, 1.0};
const rgb_color_t BUMPER_COLOR  = {0.6, 0.6, 0.6};
const rgb_color_t STAR_COLOR = {1.0, 1.0, 0.0};
const vector_t STAR_VELOCITY_INIT = {-30, 0};
const vector_t STAR_BOUNDS = {280, 770};
const vector_t STAR_SPEC = {8, 35};

// Ball constants
const double BALL_ERROR = 20.0;
const double BALL_MASS = 1.0;
const double BALL_HEIGHT = 65.0;

// Spring constants
const double SPRING_HEIGHT = 10.0;
const double SPRING_SPACE = 5.0;

// Grav constants
const double G = 6.67E11;
const double M = 6E24;
double g = 9.8;

// Swinger constants
const double SWINGER_ELASTICITY = 1.5;
const double SWINGER_HEIGHT = 150;
const double LEFT_SWINGER_POS = 380;
const double RIGHT_SWINGER_POS = 620;
const double SWINGER_LENGTH = 110;
const double LEFT_SWINGER_ANG = 11*M_PI/6;
const double RIGHT_SWINGER_ANG = 7*M_PI/6;
const rgb_color_t SWINGER_COLOR = {1, 0, 0};

const double ACC_WIDTH = 70;
const double ACC_HEIGHT = 20;
const double ACC_POS_Y = 250;

const double WALL_COLLISION = 0.4;
const double BUMPER_COLLISION = 0.9;

// POINTS STUFF
const int REG_POINTS = 500.0;
const int LEVEL_CHANGER_SCORE = 1000;

bool flung = false;
bool hit_wall = false;
bool added_grav = false;
int lives = 3;
int score = 0;

body_t *get_player(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *player = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(player))->is_player) {
            return player;
        }
    }
    return NULL;
}

body_t *get_spring(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *b = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(b))->is_spring) {
            return b;
        }
    }
    return NULL;
}


list_t *get_stars(scene_t *scene){
    list_t *star_list = list_init(1, (free_func_t) body_free);
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *b = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(b))->is_star) {
            list_add(star_list, b);
        }
    }
    return star_list;
}

body_t *get_earth(scene_t *scene){
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *b = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(b))->is_earth) {
            return b;
        }
    }
    return NULL;
}

void show_lives(scene_t *scene){
    for (int i = 0; i < lives - 1; i++){
        double spacing = 15.0;
        double ball_rad;
        body_t *l = malloc(sizeof(body_t*));
        if (lives <= 3){
            ball_rad = ((BOX_SPEC.x - 2 * SPACING_BOXES) - ((3 + 1) * spacing)) / (2 * (3));
            l = make_circle(ball_rad, 0, 2 * M_PI, (rgb_color_t) {0.4, 0.4, 0.4}, 5.0, 0);
        }
        else{
            ball_rad = ((BOX_SPEC.x - 2 * SPACING_BOXES) - (lives * spacing)) / (2 * (lives - 1));
            l = make_circle(ball_rad, 0, 2 * M_PI, (rgb_color_t) {0.4, 0.4, 0.4}, 5.0, 0);
        }
        body_set_centroid(l, (vector_t) {BOX_POINT.x - BOX_SPEC.x / 2 + 2 * SPACING_BOXES + spacing + i * (2 * ball_rad + spacing), BOX_POINT.y});
        scene_add_body(scene, l);
    }
}

void make_score_template(scene_t *scene){
    // Sets up 4 boxes
    for (int i = 0; i < 4; i++){
        body_t *box = make_box(BOX_SPEC.x, BOX_SPEC.y, BLACK, 0);
        body_t *inner_box = make_box(BOX_SPEC.x - SPACING_BOXES, BOX_SPEC.y - SPACING_BOXES, INNER_COLOR, 0);
        body_set_centroid(box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        body_set_centroid(inner_box, (vector_t) {BOX_POINT.x, BOX_POINT.y - i * (SPACING_BOX_GAP + BOX_SPEC.y)});
        scene_add_body(scene, box);
        scene_add_body(scene, inner_box);
    }
}

void make_pinball_border(scene_t *scene){
    body_t *ball = get_player(scene);
    list_t *pinball_border = list_init(1, (free_func_t) body_free);

    // Sets up pinball border
    body_t *border1 = make_box(SPACING, BOARD_SPEC.y, BLACK, 0);
    body_t *border2 = make_box(BOARD_SPEC.x, SPACING, BLACK, 0);
    body_t *border3 = make_box(SPACING, BOARD_SPEC.y, BLACK, 0);
    body_t *border4 = make_box(BOARD_SPEC.x, SPACING, BLACK, 0);
    body_set_centroid(border1, BOARD_POINT);
    body_set_centroid(border2, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2, BOARD_POINT.y + BOARD_SPEC.y/2});
    body_set_centroid(border3, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x, BOARD_POINT.y});
    body_set_centroid(border4, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2, BOARD_POINT.y - BOARD_SPEC.y/2});
    list_add(pinball_border, border1);
    list_add(pinball_border, border2);
    list_add(pinball_border, border3);
    list_add(pinball_border, border4);

    // Sets up ball alley
    double alley_top = ALLEY_POINT.y + ALLEY_SPEC.y/2;
    body_t *alley1 = make_box(SPACING, ALLEY_SPEC.y, BLACK, 0);
    body_t *alley2 = make_box(ALLEY_SPEC.x, SPACING, BLACK, 0);
    body_t *alley3 = make_box(SPACING, ALLEY_SPEC.y, BLACK, 0);
    body_set_centroid(alley1, (vector_t) {ALLEY_POINT.x - ALLEY_SPEC.x/2, ALLEY_POINT.y});
    body_set_centroid(alley2, (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y - ALLEY_SPEC.y/2});
    body_set_centroid(alley3, (vector_t) {ALLEY_POINT.x + ALLEY_SPEC.x/2, ALLEY_POINT.y});
    list_add(pinball_border, alley1);
    list_add(pinball_border, alley2);
    list_add(pinball_border, alley3);

    // Sets up segueway
    body_t *bridge = make_trapezoid(CORNER_SPEC.x/2.45, CORNER_SPEC.y/2.45, SPACING, -1, BLACK, 1);
    body_set_centroid(bridge, (vector_t) {RIGHT_WALL_POINT.x + BRIDGE_WIDTH - SPACING/2, alley_top});
    list_add(pinball_border, bridge);
    body_t *bridge2 = make_trapezoid(CORNER_SPEC.x/3, CORNER_SPEC.y/3, SPACING, 1, BLACK, 1);
    body_set_centroid(bridge2, (vector_t) {RIGHT_WALL_POINT.x - SPACING/2, alley_top});
    list_add(pinball_border, bridge2);

    // Sets up outer walls
    body_t *wall_left = make_box(SPACING, WALL_HEIGHT.x, BLACK, 0);
    body_t *wall_right = make_box(SPACING, WALL_HEIGHT.y, BLACK, 0);
    body_set_centroid(wall_left, LEFT_WALL_POINT);
    body_set_centroid(wall_right, RIGHT_WALL_POINT);
    list_add(pinball_border, wall_left);
    list_add(pinball_border, wall_right);

    // Sets up corner guards
    body_t *corner_left = make_trapezoid(CORNER_SPEC.x, CORNER_SPEC.y, SPACING, 1, BLACK, 1);
    body_t *corner_right = make_trapezoid(CORNER_SPEC.x, CORNER_SPEC.y, SPACING, -1, BLACK, 1);
    body_set_centroid(corner_left, (vector_t) {LEFT_WALL_POINT.x - CORNER_DELTA, alley_top});
    body_set_centroid(corner_right, (vector_t) {ALLEY_POINT.x  + ALLEY_SPEC.x/2 + CORNER_DELTA, alley_top});
    list_add(pinball_border, corner_left);
    list_add(pinball_border, corner_right);

    // Sets up roof
    body_t *roof = make_box(ROOF_WIDTH, SPACING, BLACK, 0);
    body_set_centroid(roof, (vector_t) {BOARD_POINT.x + BOARD_SPEC.x/2 + 2.32 * SPACING, alley_top + CORNER_SPEC.y - SPACING/2});
    list_add(pinball_border, roof);

    // Sets up cone bottom
    body_t *cone1 = make_trapezoid(CONE_SPEC.x, CONE_SPEC.y, SPACING, 1, BLACK, 1);
    body_t *cone2 = make_trapezoid(CONE_SPEC.x, CONE_SPEC.y, SPACING, -1, BLACK, 1);
    body_set_centroid(cone1, (vector_t) {CONE_POINT.x + LOSING_SPEC.x/2 - SPACING, CONE_POINT.y});
    body_set_centroid(cone2, (vector_t) {CONE_POINT.x - LOSING_SPEC.x/2 + SPACING, CONE_POINT.y});
    list_add(pinball_border, cone1);
    list_add(pinball_border, cone2);

    for (size_t i = 0; i < list_size(pinball_border); i++){
        body_t *b = list_get(pinball_border, i);
        scene_add_body(scene, b);
        create_physics_collision(scene, WALL_COLLISION, ball, b);
    }
}

/*   || BUMPER STUFF || */

void points(body_t *body1, body_t *body2, vector_t axis, void *aux){
    score += aux_get_constant(aux);
}

void extra_life(body_t *ball, body_t *bumper, vector_t axis, void *aux){
    lives += 1;

    if (body_is_removed(bumper)) return;

    body_remove(bumper);

    body_set_centroid(ball, vec_add(body_get_centroid(ball),
        vec_multiply(0.1 * 1.5, body_get_velocity(ball))));
    double bumper_radius = ALLEY_SPEC.x / 1.8 - BALL_ERROR;
    body_t *b = make_circle(bumper_radius, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_set_centroid(b, body_get_centroid(bumper));
    scene_t *scene = aux;
    scene_add_body(scene, b);
    show_lives(scene);
    create_physics_collision(scene, BUMPER_COLLISION, ball, b);
    aux_t *bumper_aux = aux_init(REG_POINTS, ball, b);
    create_collision(scene, ball, b, (collision_handler_t) points, (void*) bumper_aux, (free_func_t) aux_free);
}

// no lives lost; ball just restarts on the spring
void restart_bumper(body_t *ball, body_t *bumper, vector_t axis, void *aux){
    if (body_is_removed(bumper)) return;
    flung = false;
    added_grav = false;

    body_remove(bumper);

    // bumper reset
    double bumper_radius = ALLEY_SPEC.x / 1.8 - BALL_ERROR;
    body_set_centroid(ball, vec_add(body_get_centroid(ball),
        vec_multiply(0.1 * 1.5, body_get_velocity(ball))));
    body_t *b = make_circle(bumper_radius, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_set_centroid(b, body_get_centroid(bumper));
    scene_t *scene = aux;
    scene_add_body(scene, b);
    create_physics_collision(scene, BUMPER_COLLISION, ball, b);
    aux_t *bumper_aux = aux_init(REG_POINTS, ball, b);
    create_collision(scene, ball, b, (collision_handler_t) points, (void*) bumper_aux, (free_func_t) aux_free);

    // ball and spring reset
    body_t *earth = get_earth(scene);
    body_remove(earth);

    body_set_velocity(ball, VEC_ZERO);
    body_set_centroid(ball, vec_add(ALLEY_POINT, (vector_t) {0, BALL_HEIGHT}));
    body_t *s = get_spring(scene);
    body_remove(s);

    rgb_color_t spring_color = (rgb_color_t) {0.41, 0.41, 0.41};
    body_t *spring = make_box(ALLEY_SPEC.x - 5.0, SPRING_HEIGHT,
        spring_color, 1);
    vector_t spring_start_pos = (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y +
        BALL_HEIGHT - (ALLEY_SPEC.x / 2 - BALL_ERROR) - (SPRING_HEIGHT/ 2) - SPRING_SPACE};
    body_set_centroid(spring, spring_start_pos);
    scene_add_body(scene, spring);

    create_physics_collision(scene, 1.5, ball, spring);
}

void make_bumpers(scene_t *scene){
    double alley_top = ALLEY_POINT.y + ALLEY_SPEC.y/2;
    double bumper_radius = ALLEY_SPEC.x / 1.8 - BALL_ERROR;
    double delta_x = CONE_POINT.x - (LEFT_WALL_POINT.x - CORNER_DELTA + 15 * SPACING);
    list_t *bumper_list = list_init(1, (free_func_t) body_free);
    list_t *star_list = list_init(1, (free_func_t) body_free);
    body_t *ball = get_player(scene);

    /*
    body_t *top_bumper = make_circle(bumper_radius, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_set_centroid(top_bumper, (vector_t) {CONE_POINT.x, alley_top +  SPACING});
    list_add(bumper_list, top_bumper);
    */

    body_t *top_l_bumper = make_circle(bumper_radius, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_t *top_r_bumper = make_circle(bumper_radius, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_set_centroid(top_l_bumper, (vector_t) {CONE_POINT.x - delta_x, alley_top - 35 * SPACING});
    body_set_centroid(top_r_bumper, (vector_t) {CONE_POINT.x + delta_x, alley_top - 35 * SPACING});
    list_add(bumper_list, top_l_bumper);
    list_add(bumper_list, top_r_bumper);

    body_t *bottom_l_bumper = make_circle(bumper_radius/1.5, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_t *bottom_r_bumper = make_circle(bumper_radius/1.5, 0, 2 * M_PI, BUMPER_COLOR, INFINITY, 0);
    body_set_centroid(bottom_l_bumper, (vector_t) {CONE_POINT.x - .8 * delta_x, alley_top - 55 * SPACING});
    body_set_centroid(bottom_r_bumper, (vector_t) {CONE_POINT.x + .8 * delta_x, alley_top - 55 * SPACING});
    list_add(bumper_list, bottom_l_bumper);
    list_add(bumper_list, bottom_r_bumper);

    for (size_t i = 0; i < list_size(bumper_list); i++){
        body_t *b = list_get(bumper_list, i);
        scene_add_body(scene, b);
        create_physics_collision(scene, BUMPER_COLLISION, ball, b);
        aux_t *bumper_aux = aux_init(REG_POINTS, ball, b);
        create_collision(scene, ball, b, (collision_handler_t) points, (void*) bumper_aux, (free_func_t) aux_free);
    }

    body_t *black_hole = make_circle(bumper_radius/1.5, 0, 2 * M_PI, BLACK, INFINITY, 0);
    body_set_centroid(black_hole, (vector_t) {CONE_POINT.x - .9 * delta_x, alley_top - 45 * SPACING});
    scene_add_body(scene, black_hole);
    create_physics_collision(scene, BUMPER_COLLISION, ball, black_hole);
    create_collision(scene, ball, black_hole, (collision_handler_t) restart_bumper, scene, NULL);

    body_t *gain_life = make_circle(bumper_radius/1.5, 0, 2 * M_PI, SWINGER_COLOR, INFINITY, 0);
    body_set_centroid(gain_life, (vector_t) {CONE_POINT.x + .9 * delta_x, alley_top - 45 * SPACING});
    scene_add_body(scene, gain_life);
    create_physics_collision(scene, BUMPER_COLLISION, ball, gain_life);
    create_collision(scene, ball, gain_life, (collision_handler_t) extra_life, scene, NULL);

    // Setup stars
    for (size_t i = 0; i < 3; i++){
        body_t *s = make_star(STAR_SPEC.x, STAR_SPEC.y + 2 * i, STAR_COLOR);
        body_set_centroid(s, (vector_t) {CONE_POINT.x - (100 * i), alley_top + 30 - STAR_SPEC.y * 2.3 * i});
        body_set_rotation(s, .1);
        body_set_velocity(s, (vector_t){STAR_VELOCITY_INIT.x * (i + 1), STAR_VELOCITY_INIT.y * (i + 1)});
        scene_add_body(scene, s);
        create_physics_collision(scene, BUMPER_COLLISION, ball, s);
        aux_t *bumper_aux = aux_init(1000, ball, s);
        create_collision(scene, ball, s, (collision_handler_t) points, (void*) bumper_aux, (free_func_t) aux_free);
    }
}

void spring_bounds(scene_t *scene){
    body_t *spring = get_spring(scene);
    body_t *ball = get_player(scene);
    vector_t v = body_get_centroid(spring);
    double thickness = 13.0;
    if (!added_grav && !flung){
        body_set_velocity(ball, VEC_ZERO);
    }

    if (v.y < SEGUEWAY_LEFT_SPEC.y + thickness){
        vector_t alley_bottom = (vector_t) {ALLEY_POINT.x, SEGUEWAY_LEFT_SPEC.y + thickness};
        body_set_centroid(spring, alley_bottom);
    }
    else if (v.y >= ALLEY_POINT.y + BALL_HEIGHT && flung){
        body_set_velocity(spring, VEC_ZERO);
        if (!added_grav){
            printf("ADDING GRAVITY ----------------------\n");
            added_grav = true;
            // Gravity box !
            double R = sqrt(G * M / g);
            body_t *grav = make_circle(5.0, 0.0, 2 * M_PI, BLACK, M, 2);
            vector_t gravity_center = {.x = MAX_X / 2, .y = -R};
            body_set_centroid(grav, gravity_center);
            scene_add_body(scene, grav);
            create_newtonian_gravity(scene, G, ball, grav);
            //body_set_velocity(ball, (vector_t) {0.0, -3.0});
        }
        else if (added_grav){
            vector_t temp_v = body_get_velocity(ball);
            temp_v.y -= 1.0;
            body_set_velocity(ball, temp_v);
        }
    }
}

void spring_move(double held_time, body_t *spring){
    double speed = 50.0;
    vector_t v = vec_multiply(-1, (vector_t){0, speed});
    body_set_velocity(spring, v);
}

void spring_fling(void *key_handler_aux){
    // spring anchor
    body_t *spring = key_aux_get_spring(key_handler_aux);
    scene_t *scene = key_aux_get_scene(key_handler_aux);

    body_set_velocity(spring, VEC_ZERO);

    body_t *anchor = make_circle(5.0, 0, 2 * M_PI, BALL_COLOR, INFINITY, 0);
    body_set_centroid(anchor, vec_add(ALLEY_POINT, (vector_t) {0, BALL_HEIGHT}));
    scene_add_body(scene, anchor);
    create_spring(scene, 1000.0, spring, anchor);
}

void left_swinger(double held_time, void *key_handler_aux){
    swinger_t *s = key_aux_get_swinger1(key_handler_aux);
    double momentum = held_time * MOMENTUM_CONSTANT;
    swinger_add_momentum(s, momentum);
}

void right_swinger(double held_time, void *key_handler_aux){
        swinger_t *s2 = key_aux_get_swinger2(key_handler_aux);
        double momentum2 = -1 * held_time * MOMENTUM_CONSTANT;
        swinger_add_momentum(s2, momentum2);
}

void on_key(char key, key_event_type_t type, double held_time, void *key_handler_aux) {
    body_t *spring = key_aux_get_spring(key_handler_aux);
    if (type == KEY_PRESSED){
        switch (key) {
            case (LEFT_ARROW):
                left_swinger(held_time, key_handler_aux);
                break;
            case (RIGHT_ARROW):
                right_swinger(held_time, key_handler_aux);
                break;
            case (' '):
                if (!flung){
                    spring_move(held_time, spring);
                }
                break;
        }
        double time_divisions = 11;
        body_tick(spring, held_time / time_divisions);
    }
    else if (type == KEY_RELEASED){
        switch (key) {
            case (' '):
                if (!flung){
                    flung = true;
                    spring_fling(key_handler_aux);
                }
                break;
        }
    }
}

void reset_game(scene_t *scene){
    flung = false;
    added_grav = false;

    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_remove(scene_get_body(scene, i));
    }

    // add accelerators
    body_t *acc1 = make_accelerator(ACC_WIDTH, ACC_HEIGHT, (vector_t){MID_X, ACC_POS_Y}, (rgb_color_t) {1, 0, 1});
    scene_add_body(scene, acc1);
    body_t *acc2 = make_accelerator(ACC_WIDTH, ACC_HEIGHT, (vector_t){MID_X, ACC_POS_Y + BALL_ERROR}, (rgb_color_t) {.8, 0, .8});
    scene_add_body(scene, acc2);
    body_t *acc3 = make_accelerator(ACC_WIDTH, ACC_HEIGHT, (vector_t){MID_X, ACC_POS_Y + 2 * BALL_ERROR}, (rgb_color_t) {.6, 0, .6});
    scene_add_body(scene, acc3);

    // add ball
    body_t *ball = make_circle(ALLEY_SPEC.x / 2 - BALL_ERROR, 0, 2 * M_PI, BALL_COLOR, BALL_MASS, 1.0);
    body_set_centroid(ball, vec_add(ALLEY_POINT, (vector_t) {0, BALL_HEIGHT}));
    scene_add_body(scene, ball);

    // add spring
    rgb_color_t spring_color = (rgb_color_t) {0.41, 0.41, 0.41};
    body_t *spring = make_box(ALLEY_SPEC.x - 5.0, SPRING_HEIGHT,
        spring_color, 1);
    vector_t spring_start_pos = (vector_t) {ALLEY_POINT.x, ALLEY_POINT.y +
        BALL_HEIGHT - (ALLEY_SPEC.x / 2 - BALL_ERROR) - (SPRING_HEIGHT/ 2) - SPRING_SPACE};
    body_set_centroid(spring, spring_start_pos);
    scene_add_body(scene, spring);

    create_physics_collision(scene, 1.5, ball, spring);

    // Sets up losing area
    body_t *losing_area = make_box(LOSING_SPEC.x, LOSING_SPEC.y, BLACK, 0);
    body_set_centroid(losing_area, CONE_POINT);
    create_destructive_collision(scene, ball, losing_area);
    scene_add_body(scene, losing_area);

    make_pinball_border(scene);
    make_score_template(scene);
    make_bumpers(scene);

    show_lives(scene);
}

void check_accelerator(body_t *ball){
    vector_t c = polygon_centroid(body_get_shape(ball));
    if ((c.x < MID_X - ACC_WIDTH || c.x < MID_X + ACC_WIDTH)
    && (c.y < ACC_POS_Y - ACC_HEIGHT || c.y < ACC_POS_Y + 5 * ACC_HEIGHT)){
        body_set_velocity(ball, (vector_t) {body_get_velocity(ball).x, body_get_velocity(ball).y + 35});
    }
}

void check_star(scene_t *scene, double total_time){
    list_t *star_list = get_stars(scene);
    for (size_t i = 0; i < list_size(star_list); i++){
        body_t *star = list_get(star_list, i);
        body_set_color(star, phase_yellow(STAR_COLOR, total_time));
        double x = polygon_centroid(body_get_shape(star)).x;
        if (x < STAR_BOUNDS.x || x > STAR_BOUNDS.y){
            body_set_velocity(star,(vector_t){-1 * body_get_velocity(star).x, 0});
        }
    }
}

int main(){
    sdl_init((vector_t){MIN_XY, MIN_XY}, (vector_t){MAX_X, MAX_Y});
    scene_t *scene = scene_init();
    double total_time = 0.0;
    reset_game(scene);

    list_t *swingers = list_init(1, (free_func_t)swinger_free);
    swinger_t *s1 = swinger_init((vector_t){LEFT_SWINGER_POS, SWINGER_HEIGHT}, LEFT_SWINGER_ANG, SWINGER_LENGTH, SWINGER_COLOR);
    swinger_t *s2 = swinger_init((vector_t){RIGHT_SWINGER_POS, SWINGER_HEIGHT}, RIGHT_SWINGER_ANG, SWINGER_LENGTH, SWINGER_COLOR);
    list_add(swingers, s1);
    list_add(swingers, s2);
    int *sw1counter = malloc(sizeof(int));
    int *sw2counter = malloc(sizeof(int));
    *sw1counter = 0;
    *sw2counter = 0;

    sdl_on_key(on_key);

    double TEXT_DIST = 45.0;
    while (!sdl_is_done()){
        body_t *ball = get_player(scene);
        double dt = time_since_last_tick();
        total_time += dt;
        printf("SCORE: %d\n", score);
        printf("GRAV: %f\n", g);
        //   check_accelerator(ball);

        check_star(scene, total_time);
        // check if life lost
        if (get_player(scene) == NULL){
            lives--;
            reset_game(scene);
        }
        else{
            // PUT ANYTHING WITH BALL IN HERE -- assures no read error for null
            spring_bounds(scene);
            body_set_color(ball, phase_color(BALL_COLOR, total_time));
            temp_swinger_collision(scene, SWINGER_ELASTICITY, s1, ball, sw1counter);
            temp_swinger_collision(scene, SWINGER_ELASTICITY, s2, ball, sw2counter);
            swinger_tick(s1, dt);
            swinger_tick(s2, dt);

            // TEXT STUFF
/*            sdl_render_text((vector_t) {BOX_POINT.x - BOX_SPEC.x / 2, BOX_POINT.y - 4 * (SPACING_BOX_GAP + BOX_SPEC.y) + BOX_SPEC.y / 2 + TEXT_DIST}, 24, "Lives:", BLACK);
 *          sdl_render_text((vector_t) {BOX_POINT.x - BOX_SPEC.x / 2, BOX_POINT.y - 3 *(SPACING_BOX_GAP + BOX_SPEC.y) + BOX_SPEC.y / 2 + TEXT_DIST}, 24, "Points:", BLACK);
            char print_score[10];
            sprintf(print_score, "%d", score);
            sdl_render_text((vector_t) {BOX_POINT.x, BOX_POINT.y - 3 *(SPACING_BOX_GAP + BOX_SPEC.y) + BOX_SPEC.y / 2 + TEXT_DIST}, 24, print_score, BLACK);

            sdl_render_text((vector_t) {5.0, TEXT_DIST}, 24, "Level: ", BLACK);
            char level[10];
            sprintf(level, "%d", score / LEVEL_CHANGER_SCORE);
            sdl_render_text((vector_t) {5.0, 24 + TEXT_DIST}, 24, level, BLACK); */

            // TA Images
            //double bumper_radius = ALLEY_SPEC.x / 1.8 - BALL_ERROR;
            //sdl_render_image((vector_t) {300, 7.5 * bumper_radius});
            //sdl_render_image((vector_t) {BOX_POINT.x, BOX_POINT.y - 3 *(SPACING_BOX_GAP + BOX_SPEC.y) + BOX_SPEC.y / 2 + TEXT_DIST});
        }

        // TA Images
        double bumper_radius = ALLEY_SPEC.x / 1.8 - BALL_ERROR;
        //sdl_render_image((vector_t) {300, 7.5 * bumper_radius});
        //sdl_render_image((vector_t) {BOX_POINT.x, BOX_POINT.y - 3 *(SPACING_BOX_GAP + BOX_SPEC.y) + BOX_SPEC.y / 2 + TEXT_DIST});


        if (lives <= 0){
            // end game screen}
            printf("GAME OVER\n");
        }

        if (score % LEVEL_CHANGER_SCORE == 0 && score / LEVEL_CHANGER_SCORE != 0 && score != LEVEL_CHANGER_SCORE){
            g = (float) score / LEVEL_CHANGER_SCORE * 100 * 9.8;
        }
        scene_tick(scene, dt);
        sdl_render_scene(scene, swingers);
        sdl_clear();
    }

    swinger_free(s1);
    swinger_free(s2);
    free(sw1counter);
    free(sw2counter);
    scene_free(scene);
  }
