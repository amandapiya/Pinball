#include "key_handler_aux.h"
#include "body.h"
#include "collision.h"
#include "scene.h"
#include "swinger.h"
#include <assert.h>

typedef struct key_handler_aux{
    scene_t *scene; // set this val to anything for collisions
    swinger_t *swinger1;
    swinger_t *swinger2; // set this body to null for drag
    body_t *spring;
} key_handler_aux_t;

key_handler_aux_t *key_handler_aux_init(scene_t *scene, swinger_t *swing1, swinger_t *swing2, body_t *spring){
    key_handler_aux_t *aux = malloc(sizeof(key_handler_aux_t));
    assert(aux != NULL);
    aux->scene = scene;
    aux->swinger1 = swing1;
    aux->swinger2 = swing2;
    aux->spring = spring;
    return aux;
}

void key_aux_free(key_handler_aux_t *aux){
    free(aux);
}

/**
 * Returns the pointer to swinger1
 */
swinger_t *key_aux_get_swinger1(key_handler_aux_t *aux){
    return aux->swinger1;
}

/**
 * Returns the pointer to swinger2
 */
swinger_t *key_aux_get_swinger2(key_handler_aux_t *aux){
    return aux->swinger2;
}

/**
 * Returns the scene
 */
scene_t *key_aux_get_scene(key_handler_aux_t *aux){
    return aux->scene;
}

body_t *key_aux_get_spring(key_handler_aux_t *aux){
    return aux->spring;
}
