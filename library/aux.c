#include "aux.h"
#include <assert.h>
#include <stdlib.h>
#include "collision.h"
#include "body.h"

typedef struct aux{
    double val; // set this val to anything for collisions
    body_t *body1;
    body_t *body2; // set this body to null for drag
} aux_t;

aux_t *aux_init(double val, body_t *body1, body_t *body2){
    aux_t *aux = malloc(sizeof(aux_t));
    assert(aux != NULL);
    aux->val = val;
    aux->body1 = body1;
    aux->body2 = body2;
    return aux;
}

void aux_free(aux_t *aux){
    free(aux);
}

body_t *aux_get_body1(aux_t *aux){
    assert(aux->body1 != NULL);
    return aux->body1;
}

body_t *aux_get_body2(aux_t *aux){
    return aux->body2;
}

double aux_get_constant(aux_t *aux){
    return aux->val;
}
