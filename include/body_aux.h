#ifndef __BODY_AUX_H__
#define __BODY_AUX_H__

#include <stdbool.h>

typedef struct {
    bool is_player;
    bool is_bumper;
} body_aux_t;

extern const body_aux_t PROP;
extern const body_aux_t SPACE_ENEMY;
extern const body_aux_t SPACE_PLAYER;
extern const body_aux_t BRICK;

#endif // #ifndef __BODY_AUX_H__
