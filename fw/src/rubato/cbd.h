#ifndef CBD_H
#define CBD_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

#define cbd2 RUBATO_NAMESPACE(cbd2)
void cbd2(poly *r, const uint8_t buf[2*N/4]);

#define cbd3 RUBATO_NAMESPACE(cbd3)
void cbd3(poly *r, const uint8_t buf[3*N/4]);

#endif
