#include <stdint.h>
#include <stddef.h>
#include "params.h"
#include "rubato_soft.h"

void poly_soft_pointwise_add(poly *C, const poly *A, const poly *B);
void poly_soft_pointwise_sub(poly *C, const poly *A, const poly *B);
void poly_soft_pointwise_mult(poly *C, const poly *A, const poly *B);
void poly_soft_mult_add(poly *D, const poly *A, const poly *B, const poly *C);