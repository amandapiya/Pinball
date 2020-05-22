#ifndef __AUX_H__
#define __AUX_H__
#include "body.h"
#include "collision.h"

/**
 * Holds the variables needed to pass into a force creator function
 * Consists of a constant and two bodies
 */
typedef struct aux aux_t;

/**
 * Initializes an aux with a constant and two bodies
 *
 * @param constant the constant depending on the force
 * @param body1 a body the force acts on
 * @param body2 another body the force acts on; NULL if drag
 *
 * @return a pointer to the newly allocated aux holder
 */
aux_t *aux_init(double val, body_t *body1, body_t *body2);

/**
 * Releases the bodies in the aux and aux
 *
 * @param aux a pointer to the aux
 */
void aux_free(aux_t *aux);

/**
 * Returns the pointer to body1
 */
body_t *aux_get_body1(aux_t *aux);

/**
 * Returns the pointer to body2
 */
body_t *aux_get_body2(aux_t *aux);

/**
 * Returns the constant
 */
double aux_get_constant(aux_t *aux);

#endif // #ifndef __AUX_H__
