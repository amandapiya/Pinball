#include "forces.h"
#include "body.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "aux.h"
#include "collision.h"
#include "body_aux.h"
#include "color.h"
#include "swinger.h"
#include "collision_storage.h"

const double TOO_CLOSE_GRAV = 5.0;
const float COLOR_UPDATE = 0.1;

void gravity_force(aux_t *aux){
    double G = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    body_t *body2 = aux_get_body2(aux);
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    vector_t r2minus1= vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    double r12 = sqrt(vec_dot(r2minus1, r2minus1));
    vector_t r = vec_multiply(1.0 / r12, r2minus1);

    if (r12 > TOO_CLOSE_GRAV){
        // define how close we want bodies to be for force to be ignored
        vector_t F21 = vec_multiply((-G * m1 * m2 / (r12 * r12)), r);
        body_add_force(body2, F21);
        body_add_force(body1, vec_negate(F21));
    }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2){
    aux_t *grav_vars = aux_init(G, body1, body2);
    scene_add_force_creator(scene, (force_creator_t) gravity_force, (void*) grav_vars, (free_func_t) aux_free);
}

void spring_force(aux_t *aux){
    double k = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    body_t *body2 = aux_get_body2(aux);
    vector_t Fx = vec_multiply(k, vec_subtract(body_get_centroid(body2), body_get_centroid(body1)));
    body_add_force(body1, Fx);
    body_add_force(body2, vec_negate(Fx));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2){
    aux_t *spring_vars = aux_init(k, body1, body2);
    scene_add_force_creator(scene, (force_creator_t) spring_force, (void*) spring_vars, (free_func_t) aux_free);
}

void drag_force(aux_t *aux){
    double gamma = aux_get_constant(aux);
    body_t *body1 = aux_get_body1(aux);
    vector_t v = body_get_velocity(body1);
    body_add_force(body1, vec_multiply(-gamma, v));
}

void create_drag(scene_t *scene, double gamma, body_t *body){
    aux_t *drag_vars = aux_init(gamma, body, NULL);
    scene_add_force_creator(scene, (force_creator_t) drag_force, (void*) drag_vars, (free_func_t) aux_free);
}

//collision_handler_t
void destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux){
    body_remove(body1);
    body_remove(body2);
}

void physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    double elasticity_val = aux_get_constant(aux);
    double player_dot_product = vec_dot(body_get_velocity(body1), axis);
    double enemy_dot_product = vec_dot(body_get_velocity(body2), axis);

    if (body_get_mass(body2) == INFINITY) {
        double impulse = body_get_mass(body1) * (1 + elasticity_val) * (enemy_dot_product - player_dot_product);
        body_add_impulse(body1, vec_multiply(impulse, axis));
    }
    else {
        double reduced_mass = body_get_mass(body1) * body_get_mass(body2) / (body_get_mass(body1) + body_get_mass(body2));
        double impulse = reduced_mass * (1 + elasticity_val) * (enemy_dot_product - player_dot_product);
        body_add_impulse(body1, vec_multiply(impulse, axis));
        body_add_impulse(body2, vec_multiply(-1 *impulse, axis));
    }
}

void collision(void *collision_storage){
    body_t *body1 = collision_storage_get_body1(collision_storage);
    body_t *body2 = collision_storage_get_body2(collision_storage);

    collision_info_t *collision = find_collision(body_get_shape(body1), body_get_shape(body2));
    if (collision->collided){
        if (!get_prev_collision(collision_storage)){
            collision_handler_t handler = (collision_handler_t) collision_storage_get_handler(collision_storage);
            handler(body1, body2,collision->axis, collision_storage_get_aux(collision_storage));
            set_prev_collision(collision_storage, true);
        }
        else{
            set_prev_collision(collision_storage, false);
        }
    }
}

// general function -- handlers are functions
void create_collision(scene_t *scene, body_t *body1, body_t *body2, collision_handler_t handler, void *aux, free_func_t freer) {
    list_t *bodies = list_init(2, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    collision_storage_t *col_stor = collision_storage_init(aux, body1, body2, handler, freer);
    scene_add_bodies_force_creator(scene, (force_creator_t) collision, (void*) col_stor, bodies, (free_func_t) collision_storage_free);

}

// calls create_collision with destructive_collision as handler
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
    aux_t *destroy_vars = aux_init(0.0, body1, body2);
    create_collision(scene, body1, body2, (collision_handler_t) destructive_collision,
        (void*) destroy_vars, (free_func_t) aux_free);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1, body_t *body2) {
    aux_t *physics_vars = aux_init(elasticity, body1, body2);
    create_collision(scene, body1, body2, (collision_handler_t) physics_collision,
        (void*) physics_vars, (free_func_t) aux_free);
}

void temp_swinger_collision(scene_t *scene, double elasticity, swinger_t *swinger, body_t *ball){
    // printf("made collision\n");
    list_t *triangle = list_init(1, NULL);
    list_add(triangle, list_get(swinger_get_shape(swinger), 0));
    list_add(triangle, list_get(swinger_get_shape(swinger), 1));
    list_add(triangle, list_get(swinger_get_shape(swinger), list_size(swinger_get_shape(swinger)) - 1));
    collision_info_t *testc = find_collision(triangle, body_get_shape(ball));
    if (testc->collided == true){
       //  printf("triangle collision\n");
    }

    collision_info_t *c_info = find_collision(body_get_shape(ball), swinger_get_shape(swinger)); // maybe switch order
    if (c_info->collided == true){
        // printf("COLLISION\n");
        printf("x: %f, y: %f\n", c_info->axis.x, c_info->axis.y);
        double player_dot = vec_dot(body_get_velocity(ball), c_info->axis);
        double swinger_dot = vec_dot((vector_t){swinger_get_momentum(swinger), swinger_get_momentum(swinger)}, c_info->axis); // maybe change value
        double impulse = .5 *  (1 + elasticity) * (swinger_dot - player_dot);// last term..
        body_add_impulse(ball, vec_multiply(impulse, c_info->axis));
    }
    free(testc);
    free(c_info);
}
