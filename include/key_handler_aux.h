#ifndef __KEY_HANDLER_AUX_H__
#define __KEY_HANDLER_AUX_H__
#include "body.h"
#include "collision.h"
#include "scene.h"
#include "swinger.h"

typedef struct key_handler_aux key_handler_aux_t;

key_handler_aux_t *key_handler_aux_init(scene_t *scene, swinger_t *swing1, swinger_t *swing2, body_t *spring);

/**
 * Releases the bodies in the aux and aux
 *
 * @param aux a pointer to the aux
 */
void key_aux_free(key_handler_aux_t *aux);

/**
 * Returns the pointer to swinger1
 */
swinger_t *key_aux_get_swinger1(key_handler_aux_t *aux);

/**
 * Returns the pointer to swinger2
 */
swinger_t *key_aux_get_swinger2(key_handler_aux_t *aux);

/**
 * Returns the scene
 */
scene_t *key_aux_get_scene(key_handler_aux_t *aux);

body_t *key_aux_get_spring(key_handler_aux_t *aux);
#endif // #ifndef __KEY_HANDLER_AUX_H__
