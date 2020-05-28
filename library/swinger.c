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

typedef struct swinger {
    list_t *shape;
    rgb_color_t color;
    vector_t center;
    double angle;
    double length; // distance from swinger center to outer point
    vector_t torque;
    vector_t force;
    vector_t momentum; // of the angular variety :)
} swinger_t;

swinger_t *swinger_init(vector_t center, double angle, double length, rgb_color_t color){
    swinger_t *new = malloc(sizeof(swinger_t));
    assert(new != NULL);
    new->color = color;
    new->center = center;
    new->angle =  angle;
    new->length = length;
    new->torque = VEC_ZERO;
    new->force = VEC_ZERO;
    new->momentum = VEC_ZERO;

    // make the shape, semi-circle + the outer point
    // TODO
    new->shape = NULL;
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


double swinger_get_torque(swinger_t *swinger){
    return swinger->torque;
}

rgb_color_t swinger_get_color(swinger_t *swinger){
    return swinger->color;
}

vector_t swinger_get_momentum(swinger_t *swinger){
    return swinger->momentum;
}

void swinger_set_color(swinger_t *swinger, rgb_color_t new_color){
    swinger->color = new_color;
}

void swinger_set_torque(swinger_t *swinger, double t){
    swinger->torque = t;
}

void swinger_set_angle(swinger_t *swinger, double angle){
    // TODO
}

void swinger_add_force(swinger_t *swinger, vector_t force){
    // Not sure if needed, maybe remove
}

void swinger_add_momentum(swinger_t *swinger, double m){
    swinger->momentum += m;
}

void swinger_tick(swinger_t *swinger, double dt){
    // TODO!
}
