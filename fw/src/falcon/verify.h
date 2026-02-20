#ifndef VERIFY_H
#define VERIFY_H


#include "params.h"
#include "poly.h"

#define verify_raw FALCON_NAMESPACE(verify_raw)
int verify_raw(const poly *c0, const poly *s2, poly *h);

#endif
