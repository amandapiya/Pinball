#ifndef __SWINGER_H__
#define __SWINGER_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "vector.h"

/**
 * A swinging carrot-shaped swinger, composed of a semi-circle and a triangle
   All swingers have mass Infinity
 */
typedef struct swinger swinger_t;

/**
 * Initializes a swinger with a given center and angle of orientation.
   The point of the swinger will be at the direction of angle.
 */
swinger_t *swinger_init(vector_t center, double angle, double length, rgb_color_t color);

/**
 * Releases the memory allocated for a swinger.
 *
 * @param swinger a pointer to a swinger returned from swinger_init()
 */
void swinger_free(swinger_t *swinger);

/**
 * Gets the current shape of a swinger.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param swinger a pointer to a swinger returned from swinger_init()
 * @return the polygon describing the swinger's current position
 */
list_t *swinger_get_shape(swinger_t *swinger);

/**
 * Gets the vector corresponding to the coordinates of the semi-circle center
 */
vector_t swinger_get_center(swinger_t *swinger);

/**
 * Gets the current torque of a swinger.
 *
 * @param swinger a pointer to a swinger returned from swinger_init()
 * @return the swinger's torque
    torque is positive for CCW
    torque is negative for CW
 */
double swinger_get_torque(swinger_t *swinger);

/**
 * Gets the display color of a swinger.
 *
 * @param swinger a pointer to a swinger returned from swinger_init()
 * @return the color passed to swinger_init(), as an (R, G, B) tuple
 */
rgb_color_t swinger_get_color(swinger_t *swinger);

/**
 * Gets the angular momentum associated with a swinger.
 */
vector_t swinger_get_momentum(swinger_t *swinger);

void swinger_set_color(swinger_t *swinger, rgb_color_t new_color);

/**
 * Changes a swinger's torque
 */
void swinger_set_torque(swinger_t *swinger, double t);

/**
 * Changes a swinger's orientation in the plane.
 * The swinger is modified so it points towards the given angle.
 */
void swinger_set_angle(swinger_t *swinger, double angle);

/**
 * Applies a force to a swinger over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the swinger's position or velocity; see swinger_tick().
 *
 * @param swinger a pointer to a swinger returned from swinger_init()
 * @param force the force vector to apply
 */
void swinger_add_force(swinger_t *swinger, vector_t force);

/**
 * Applies an angular momentum to a swinger.
 * An angular momentum causes an instantaneous change in torque
 */
void swinger_add_momentum(swinger_t *swinger, double m);

/**
 * Updates the swinger after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the swinger during the tick.
 * The swinger should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the swinger.
 *
 * @param swinger the swinger to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void swinger_tick(swinger_t *swinger, double dt);

#endif // #ifndef __swinger_H__
