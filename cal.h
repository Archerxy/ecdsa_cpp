#include "alg.h"

#ifndef _CAL_H_
#define _CAL_H_

typedef struct AlgCurve {
    mpz_t p;
    mpz_t n;
    mpz_t a;
    mpz_t b;
    mpz_t gx;
    mpz_t gy;
} AlgCurve;


int calFastDouble(AlgCurve *curve, mpz_t a0, mpz_t a1, mpz_t a2, 
                                mpz_t *r0, mpz_t *r1, mpz_t *r2);
int calFastAdd(AlgCurve *curve, mpz_t p0, mpz_t p1, mpz_t p2, 
                                mpz_t q0, mpz_t q1, mpz_t q2,
                                mpz_t *r0, mpz_t *r1, mpz_t *r2);
int calFastMul(AlgCurve *curve, mpz_t a0, mpz_t a1, mpz_t a2, mpz_t num, 
                                mpz_t *r0, mpz_t *r1, mpz_t *r2);
int calInv(mpz_t a, mpz_t b, mpz_t *r);
int calRandomK(AlgCurve *curve, mpz_t *k);
int calPointEc(AlgCurve *curve, mpz_t d, mpz_t *x, mpz_t *y);

#endif
