#include "cal.h"
#include "alg.h"


#define HEX_CHECK(h) ((48<=h)&&(h<=57)||(65<=h&&h<=57)||(97<=h&&h<=102))

int calHexToBytes(const char *hex, unsigned int hex_len, uint8_t *bytes) {
    int offset = 0;
    if(hex_len & 0b1 == 1) {
        bytes[0] = hex[0];
        offset = 1;
    }
    uint8_t map[103] = {0};
    //0 - 9
    for(int i = 48; i <= 57; i++) {
        map[i] = i - 48;
    }
    //a - f
    for(int i = 65; i <= 70; i++) {
        map[i] = i - 55;
    }
    //A - F
    for(int i = 97; i <= 102; i++) {
        map[i] = i - 87;
    }
    for(int i = offset; i < (hex_len>>1); i++) {
        if(!HEX_CHECK(hex[i*2]) || !HEX_CHECK(hex[i*2+1])) {
            return 0;
        }
        bytes[i] = (map[hex[i*2]]<<4) | (map[hex[i*2+1]]);
    }
    return 1;
}

int calBytesToHex(uint8_t *bytes, unsigned int bytes_len, char *hex) {
    char map[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    for(int i = 0; i < bytes_len; i++) {
        hex[i*2] = map[(bytes[i]>>4)&0xf];
        hex[i*2+1] = map[bytes[i]&0xf];
    }
    return 1;
}

int calFastDouble(AlgCurve *curve, mpz_t a0, mpz_t a1, mpz_t a2, mpz_t *r0, mpz_t *r1, mpz_t *r2) {mpz_t ysq, s, m, tmp0, tmp1;
    mpz_init(ysq);
    mpz_init(s);
    mpz_init(m);
    mpz_init(tmp0);
    mpz_init(tmp1);
    //get ysq
    mpz_powm_ui(ysq, a1, 2l, curve->p);
    mpz_mod(ysq, ysq, curve->p);
    //get s
    mpz_mul(tmp0, ysq, a0);
    mpz_mul_ui(tmp0, tmp0, 4l);
    mpz_mod(s, tmp0, curve->p);
    //get m
    mpz_pow_ui(tmp0, a0, 2l);
    mpz_mul_ui(tmp0, tmp0, 3l);
    mpz_pow_ui(tmp1, a2, 4l);
    mpz_mod(tmp1, tmp1, curve->p);
    mpz_mul(tmp1, tmp1, curve->a);
    mpz_add(tmp0, tmp0, tmp1);
    mpz_mod(m, tmp0, curve->p);

    mpz_pow_ui(tmp0, m, 2l);
    mpz_mul_ui(tmp1, s, 2l);
    mpz_sub(*r0, tmp0, tmp1);
    mpz_mod(*r0, *r0, curve->p);

    mpz_sub(tmp0, s, *r0);
    mpz_mul(tmp0, tmp0, m);
    mpz_mod(tmp0, tmp0, curve->p);
    mpz_pow_ui(tmp1, ysq, 2l);
    mpz_mod(tmp1, tmp1, curve->p);
    mpz_mul_ui(tmp1, tmp1, 8l);
    mpz_sub(*r1, tmp0, tmp1);
    mpz_mod(*r1, *r1, curve->p);

    mpz_mul(tmp0, a1, a2);
    mpz_mul_ui(tmp0, tmp0, 2l);
    mpz_mod(*r2, tmp0, curve->p);

    mpz_clear(ysq);
    mpz_clear(s);
    mpz_clear(m);
    mpz_clear(tmp0);
    mpz_clear(tmp1);

    return 1;
}

int calFastAdd(AlgCurve *curve, mpz_t p0, mpz_t p1, mpz_t p2, 
                                   mpz_t q0, mpz_t q1, mpz_t q2,
                                   mpz_t *r0, mpz_t *r1, mpz_t *r2) {
    mpz_t u1, u2, s1, s2;
    mpz_init(u1);
    mpz_init(u2);
    mpz_init(s1);
    mpz_init(s2);

    mpz_pow_ui(u1, q2, 2l);
    mpz_mul(u1, u1, p0);
    mpz_mod(u1, u1, curve->p);
    
    mpz_pow_ui(u2, p2, 2l);
    mpz_mul(u2, u2, q0);
    mpz_mod(u2, u2, curve->p);
    
    mpz_pow_ui(s1, q2, 3l);
    mpz_mul(s1, s1, p1);
    mpz_mod(s1, s1, curve->p);
    
    mpz_pow_ui(s2, p2, 3l);
    mpz_mul(s2, s2, q1);
    mpz_mod(s2, s2, curve->p);

    if(mpz_cmp(u1, u2) == 0) {
        int ret = 0;
        if(mpz_cmp(s1, s2) == 0) {
            ret = calFastDouble(curve, p0, p1, p2, r0, r1, r2);
        } else {
            mpz_set_ui(*r0, 0l);
            mpz_set_ui(*r1, 0l);
            mpz_set_ui(*r2, 1l);
            ret = 1;
        }
        
        mpz_clear(u1);
        mpz_clear(u2);
        mpz_clear(s1);
        mpz_clear(s2);
        return ret;
    }
    
    mpz_t r, h, tmp0, tmp1, tmp2, tmp3;
    mpz_init(r);
    mpz_init(h);
    mpz_init(tmp0);
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(tmp3);

    mpz_sub(h, u2, u1);
    mpz_sub(r, s2, s1);

    mpz_pow_ui(tmp0, h, 2l);
    mpz_mod(tmp0, tmp0, curve->p);

    mpz_mul(tmp1, tmp0, h);
    mpz_mod(tmp1, tmp1, curve->p);

    mpz_mul(tmp2, u1, tmp0);
    mpz_mod(tmp2, tmp2, curve->p);

    mpz_pow_ui(tmp3, r, 2l);
    mpz_sub(tmp3, tmp3, tmp1);
    mpz_mul_ui(tmp0, tmp2, 2l);
    mpz_sub(*r0, tmp3, tmp0);
    mpz_mod(*r0, *r0, curve->p);

    mpz_sub(tmp0, tmp2, *r0);
    mpz_mul(tmp3, r, tmp0);
    mpz_mul(tmp0, s1, tmp1);
    mpz_sub(*r1, tmp3, tmp0);
    mpz_mod(*r1, *r1, curve->p);

    mpz_mul(tmp3, h, p2);
    mpz_mul(tmp3, tmp3, q2);
    mpz_mod(*r2, tmp3, curve->p);

    mpz_clear(u1);
    mpz_clear(u2);
    mpz_clear(s1);
    mpz_clear(s2);
    mpz_clear(r);
    mpz_clear(h);
    mpz_clear(tmp0);
    mpz_clear(tmp1);
    mpz_clear(tmp2);
    mpz_clear(tmp3);

    return 1;
}


int calFastMul(AlgCurve *curve, mpz_t a0, mpz_t a1, mpz_t a2, mpz_t num, mpz_t *r0, mpz_t *r1, mpz_t *r2) {
    if(mpz_cmp_ui(a1, 0l) == 0 || mpz_cmp_ui(num, 0l) == 0) {
        mpz_set_ui(*r0, 0l);
        mpz_set_ui(*r1, 0l);
        mpz_set_ui(*r2, 1l);
        return 1;
    }
    if(mpz_cmp_ui(num, 1l) == 0) {
        mpz_set(*r0, a0);
        mpz_set(*r1, a1);
        mpz_set(*r2, a2);
        return 1;
    }
    if(mpz_cmp_ui(num, 0l) < 0 || mpz_cmp(num, curve->n) >= 0) {
        mpz_mod(num, num, curve->n);
    }
    // uint32_t bits = mpz_sizeinbase(num, 2);
    const char *bits = mpz_get_str(NULL, 2, num);
    uint32_t len = strlen(bits);
    mpz_t mul0, mul1, mul2;
    mpz_init_set(mul0, a0);
    mpz_init_set(mul1, a1);
    mpz_init_set(mul2, a2);
    for(int i = 1 ; i  < len; i++) {
        calFastDouble(curve, mul0, mul1, mul2, r0, r1, r2);
        if(bits[i] == '1') {
            mpz_set(mul0, *r0);
            mpz_set(mul1, *r1);
            mpz_set(mul2, *r2);
            calFastAdd(curve, mul0, mul1, mul2, a0, a1, a2, r0, r1, r2);
        }
        mpz_set(mul0, *r0);
        mpz_set(mul1, *r1);
        mpz_set(mul2, *r2);
    }
    mpz_clear(mul0);
    mpz_clear(mul1);
    mpz_clear(mul2);
    return 1;
}


int calInv(mpz_t a, mpz_t b, mpz_t *r) {
    if(mpz_cmp_ui(a, 0) == 0) {
        mpz_set_ui(*r, 0l);
        return 1;
    }
    mpz_t lm, hm, low, high, tmp0, tmp1;
    mpz_init(lm);
    mpz_init(hm);
    mpz_init(low);
    mpz_init(high);
    mpz_init(tmp0);
    mpz_init(tmp1);

    mpz_set_ui(lm, 1l);
    mpz_set_ui(hm, 0l);
    mpz_mod(low, a, b);
    mpz_set(high, b);

    while(mpz_cmp_ui(low, 1l) > 0) {
        mpz_fdiv_q(tmp0, high, low);
        mpz_mul(tmp0, tmp0, lm);
        mpz_sub(tmp0, hm , tmp0);

        mpz_mod(tmp1, high, low);

        mpz_set(hm, lm);
        mpz_set(high, low);
        mpz_set(lm, tmp0);
        mpz_set(low, tmp1);
    }

    mpz_mod(*r, lm, b);
    
    mpz_clear(lm);
    mpz_clear(hm);
    mpz_clear(low);
    mpz_clear(high);
    mpz_clear(tmp0);
    mpz_clear(tmp1);
    return 1;
}


int calRandomK(AlgCurve *curve, mpz_t *k) {
    uint8_t r[32];
    char hex[65];
    calBytesToHex(r, 32, hex);
    hex[64] = '\0';
    hex[0] = '4';
    mpz_init_set_str(*k, hex, 16);
    return 1;
}

int calPointEc(AlgCurve *curve, mpz_t d, mpz_t *x, mpz_t *y) {
    if(mpz_cmp(d, curve->n) > 0) {
        return 0;
    }
    mpz_t r0, r1, r2, one, z;
    mpz_init(r0);
    mpz_init(r1);
    mpz_init(r2);
    mpz_init_set_ui(one, 1l);
    mpz_init(z);
    mpz_init(*x);
    mpz_init(*y);
    calFastMul(curve, curve->gx, curve->gy, one, d, &r0, &r1, &r2);

    calInv(r2, curve->p, &z);

    mpz_pow_ui(*x, z, 2l);
    mpz_mul(*x, *x, r0);
    mpz_mod(*x, *x, curve->p);

    mpz_pow_ui(*y, z, 3l);
    mpz_mul(*y, *y, r1);
    mpz_mod(*y, *y, curve->p);

    mpz_clear(r0);
    mpz_clear(r1);
    mpz_clear(r2);
    mpz_clear(one);
    mpz_clear(z);

    return 1;
}