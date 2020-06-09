#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include "swinger.h"
#include <stdbool.h>

const int RADIUS = 20;
const double INCREMENT = 0.1;
const double SWING_EPSILON = 0.01;
const int SWINGER_MASS = 3;
const int GRAVITY = 10;
const double TORQUE_CAP = 10; // ADJUSTABLE
const double SEMICIRCLE_MIN = M_PI/2;
const double SEMICIRCLE_MAX = 3*M_PI/2;
const double LEFT_SWINGER_ANGLE = 11*M_PI/6;
const double RIGHT_SWINGER_ANGLE = 7*M_PI/6;
const int MOMENTUM_EXPONENT = 3;
const int QUARTER_ANGLE = M_PI/2;

typedef struct swinger {
    list_t *shape;
    rgb_color_t color;
    vector_t center;
    double start_angle;
    double angle;
    double length; // distance from swinger center to outer point
    double torque;
    vector_t force;
    double momentum; // angular
} swinger_t;

list_t *make_shape(vector_t center, double angle, double length){
    list_t *vertices = list_init(1, free);
    // makes tip
    vector_t *vertex_add = malloc(sizeof(vector_t));
    vertex_add->x = cos(angle) * length;
    vertex_add->y = sin(angle) * length;
    vector_t *point = malloc(sizeof(vector_t));
    *point = vec_add(center, *vertex_add);
    list_add(vertices, point);
    free(vertex_add);
    // makes semi-circle
    double theta = angle + SEMICIRCLE_MIN;
    while (theta < angle + SEMICIRCLE_MAX){
        vector_t *vertex_add = malloc(sizeof(vector_t));
        vertex_add->x = cos(theta) * RADIUS;
        vertex_add->y = sin(theta) * RADIUS;
        vector_t *point = malloc(sizeof(vector_t));
        *point = vec_add(center, *vertex_add);
        list_add(vertices, point);
        free(vertex_add);
        theta += INCREMENT;
    }
    return vertices;
}

swinger_t *swinger_init(vector_t center, double angle, double length, rgb_color_t color){
    swinger_t *new = malloc(sizeof(swinger_t));
    assert(new != NULL);
    new->color = color;
    new->center = center;
    new->angle =  angle;
    new->length = length;
    new->start_angle = angle;
    new->torque = 0;
    new->force = VEC_ZERO;
    new->momentum = 0;
    new->shape = make_shape(center, angle, length);
    return new;
}

void swinger_free(swinger_t *swinger){
    list_free(swinger->shape);
    free(swinger);
}

list_t *swinger_get_shape(swinger_t *swinger){
    list_t *copy = list_init(list_size(swinger->shape), free);
    for (size_t i = 0; i < list_size(swinger->shape); i++) {
        vector_t *v = malloc(sizeof(vector_t));
        *v = *((vector_t *)list_get(swinger->shape, i));
        list_add(copy, v);
    }
    return copy;
}

vector_t swinger_get_center(swinger_t *swinger){
    return swinger->center;
}

double swinger_get_torque(swinger_t *swinger){
    return swinger->torque;
}

rgb_color_t swinger_get_color(swinger_t *swinger){
    return swinger->color;
}

double swinger_get_momentum(swinger_t *swinger){
    return swinger->momentum;
}

void swinger_set_color(swinger_t *swinger, rgb_color_t new_color){
    swinger->color = new_color;
}

void swinger_set_torque(swinger_t *swinger, double t){
    swinger->torque = t;
}

void swinger_add_momentum(swinger_t *swinger, double m){
    swinger->momentum += m;
}

void swinger_tick(swinger_t *swinger, double dt){
    swinger->torque = swinger->torque + (swinger->momentum/SWINGER_MASS);
    if (swinger->torque > TORQUE_CAP){ // SWINGER SPEED CAP
	swinger->torque = TORQUE_CAP;
    }
    double rotation_angle = dt * swinger->torque;
    swinger->momentum = 0;
    if ((fabs(swinger->start_angle - LEFT_SWINGER_ANGLE) < SWING_EPSILON && swinger->angle < LEFT_SWINGER_ANGLE) ||
         (fabs(swinger->start_angle - RIGHT_SWINGER_ANGLE) < SWING_EPSILON && swinger->angle > RIGHT_SWINGER_ANGLE)){ // left swinger
        swinger->torque = 0;
        swinger->angle = swinger->start_angle;
        list_free(swinger->shape);
        swinger->shape = make_shape(swinger->center, swinger->start_angle, swinger->length);
    }
    else if (fabs(rotation_angle) > 0){ // if torque is not zero
        if (fabs(swinger->angle - swinger->start_angle) > QUARTER_ANGLE){ // if swinger has moved too far up
            list_free(swinger->shape);
            if (swinger->angle < swinger->start_angle){
                swinger->angle = swinger->start_angle - QUARTER_ANGLE;
                swinger->shape = make_shape(swinger->center, swinger->start_angle - QUARTER_ANGLE, swinger->length);
            }
            else {
                swinger->angle = swinger->start_angle + QUARTER_ANGLE;
                swinger->shape = make_shape(swinger->center, swinger->start_angle + QUARTER_ANGLE, swinger->length);
            }
        }
        else { // regular swing
            swinger->angle = swinger->angle + rotation_angle;
            list_free(swinger->shape);
            swinger->shape = make_shape(swinger->center, swinger->angle + rotation_angle, swinger->length);
        }
    }

    // create momentum to move swinger back to original position
    if (fabs(swinger->angle - swinger->start_angle) > INCREMENT){
        double gravity_momentum = GRAVITY * pow(swinger->start_angle - swinger->angle, MOMENTUM_EXPONENT);
        swinger_add_momentum(swinger, gravity_momentum);
    }
}
