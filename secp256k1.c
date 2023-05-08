#include "alg.h"
#include "cal.h"

extern int calHexToBytes(const char *hex, unsigned int hex_len, uint8_t *bytes);
extern int calBytesToHex(uint8_t *bytes, unsigned int bytes_len, char *hex);

void ecInitBasePoint(AlgCurve *curve) {
    mpz_init_set_str(curve->p,
        "115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
    mpz_init_set_str(curve->n,
        "115792089237316195423570985008687907852837564279074904382605163141518161494337", 10);
    mpz_init_set_str(curve->a, 
        "0", 10);
    mpz_init_set_str(curve->b, 
        "7", 10);
    mpz_init_set_str(curve->gx,
        "55066263022277343669578718895168534326250603453777594175500187360389116729240", 10);
    mpz_init_set_str(curve->gy,
        "32670510020758816978083085130507043184471273380659243275938904335757337482424", 10);
}

void ecFreeBasePoint(AlgCurve *curve) {
    mpz_clear(curve->p);
    mpz_clear(curve->n);
    mpz_clear(curve->a);
    mpz_clear(curve->b);
    mpz_clear(curve->gx);
    mpz_clear(curve->gy);
}



int ecPrivateKeyToPublicKey(const char *priv_key_hex, char *pub_key_hex) {
    int ret = 0;
    AlgCurve curve;
    ecInitBasePoint(&curve);

    mpz_t private_key_num;
    ret = mpz_init_set_str(private_key_num, priv_key_hex, 16) == 0;

    if(!ret || mpz_cmp(private_key_num, curve.n) > 0) {
        ecFreeBasePoint(&curve);
        mpz_clear(private_key_num);
        return 0;
    }

    mpz_t x, y;
    mpz_init(x);
    mpz_init(y);

    calPointEc(&curve, private_key_num, &x, &y);

    char *x_hex = mpz_get_str(NULL, 16, x);
    char *y_hex = mpz_get_str(NULL, 16, y);
    unsigned int x_len = strlen(x_hex), y_len = strlen(y_hex);
    memset(pub_key_hex, '0', 128);
    memcpy(&pub_key_hex[64 - x_len], x_hex, x_len);
    memcpy(&pub_key_hex[128 - y_len], y_hex, y_len);

    ecFreeBasePoint(&curve);
    mpz_clear(private_key_num);
    mpz_clear(x);
    mpz_clear(y);
    return 1;
}

int ecSign(const char *priv_key_hex, uint8_t *content, uint32_t len, char *sig_hex) {
    int ret = 0;
    AlgCurve curve;
    mpz_t private_key_num, h;
    char h_hex[len * 2 + 1];
    uint8_t priv[32];

    ecInitBasePoint(&curve);
    calBytesToHex(content, len, h_hex);
    h_hex[len * 2] = '\0';
    ret = calHexToBytes(priv_key_hex, 64, priv);
    ret = mpz_init_set_str(private_key_num, priv_key_hex, 16) == 0;
    ret = mpz_init_set_str(h, h_hex, 16) == 0;
    if(!ret || mpz_cmp(private_key_num, curve.n) > 0) {
        ecFreeBasePoint(&curve);
        mpz_clear(h);
        mpz_clear(private_key_num);
        return 0;
    }

    mpz_t z, k, one, p0, p1, p2, r, s;
    mpz_init(z);
    mpz_init(k);
    mpz_init_set_ui(one, 1l);
    mpz_init(p0);
    mpz_init(p1);
    mpz_init(p2);
    mpz_init(r);
    mpz_init(s);
    calRandomK(&curve, &k);

    calFastMul(&curve, curve.gx, curve.gy, one, k, &p0, &p1, &p2);
    calInv(p2, curve.p, &z);

    mpz_pow_ui(r, z, 2l);
    mpz_mul(r, r, p0);
    mpz_mod(r, r, curve.p);
    
    mpz_pow_ui(p2, z, 3l);
    mpz_mul(p2, p2, p1);
    mpz_mod(p2, p2, curve.p);

    calInv(k, curve.n, &s);
    mpz_mul(k, r, private_key_num);
    mpz_add(k, k, h);
    mpz_mul(s, s, k);
    mpz_mod(s, s, curve.n);

    int v = mpz_odd_p(p2);
    mpz_mul_ui(k, s, 2l);
    if(mpz_cmp(k, curve.n) < 0) {
        v ^= 0;
    } else {
        v ^= 1;
        mpz_sub(s, curve.n, s);
    }

    char *r_hex = mpz_get_str(NULL, 16, r);
    char *s_hex = mpz_get_str(NULL, 16, s);
    unsigned int r_len = strlen(r_hex), s_len = strlen(s_hex);
    memset(sig_hex, '0', 130);
    memcpy(&sig_hex[64 - r_len], r_hex, r_len);
    memcpy(&sig_hex[128 - s_len], s_hex, s_len);
    sig_hex[128] = '0';
    sig_hex[129] = v ? '1' : '0';

    ecFreeBasePoint(&curve);
    mpz_clear(private_key_num);
    mpz_clear(h);
    mpz_clear(z);
    mpz_clear(k);
    mpz_clear(one);
    mpz_clear(p0);
    mpz_clear(p1);
    mpz_clear(p2);
    mpz_clear(r);
    mpz_clear(s);
    
    return 1;
}

int ecVerify(const char *pub_key_hex, uint8_t *content, uint32_t len, const char *sig_hex) {
    int ret = 0;
    mpz_t x, y, r, s, h;
    AlgCurve curve;
    char x_hex[65] = {'\0'}, y_hex[65] = {'\0'};
    char r_hex[65] = {'\0'}, s_hex[65] = {'\0'}, h_hex[len * 2 + 1];
    h_hex[len * 2] = '\0';


    memcpy(x_hex, pub_key_hex, 64);
    memcpy(y_hex, pub_key_hex+64, 64);
    memcpy(r_hex, sig_hex, 64);
    memcpy(s_hex, sig_hex+64, 64);
    calBytesToHex(content, len, h_hex);
    ecInitBasePoint(&curve);

    ret = mpz_init_set_str(x, x_hex, 16) == 0;
    ret = mpz_init_set_str(y, y_hex, 16) == 0;
    ret = mpz_init_set_str(r, r_hex, 16) == 0;
    ret = mpz_init_set_str(s, s_hex, 16) == 0;
    ret = mpz_init_set_str(h, h_hex, 16) == 0;
    
    if(!ret || mpz_cmp(r, curve.n) == 0 || mpz_cmp(s, curve.n) == 0) {
        ecFreeBasePoint(&curve);
        mpz_clear(x);
        mpz_clear(y);
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(h);
        return 0;
    }

    mpz_t z, one, p0, p1, p2, tx, ty, px, py, qx, qy;
    mpz_init(z);
    mpz_init_set_ui(one, 1l);
    mpz_init(p0);
    mpz_init(p1);
    mpz_init(p2);
    mpz_init(tx);
    mpz_init(ty);
    mpz_init(px);
    mpz_init(py);
    mpz_init(qx);
    mpz_init(qy);
    
    calInv(s, curve.n, &z);

    mpz_mul(tx, h, z);
    mpz_mod(tx, tx, curve.n);

    mpz_mul(ty, r, z);
    mpz_mod(ty, ty, curve.n);

    calFastMul(&curve, curve.gx, curve.gy, one, tx, &p0, &p1, &p2);
    calInv(p2, curve.p, &z);

    mpz_pow_ui(px, z, 2l);
    mpz_mul(px, px, p0);
    mpz_mod(px, px, curve.p);
    
    mpz_pow_ui(py, z, 3l);
    mpz_mul(py, py, p1);
    mpz_mod(py, py, curve.p);
    
    calFastMul(&curve, x, y, one, ty, &p0, &p1, &p2);
    calInv(p2, curve.p, &z);

    mpz_pow_ui(qx, z, 2l);
    mpz_mul(qx, qx, p0);
    mpz_mod(qx, qx, curve.p);
    
    mpz_pow_ui(qy, z, 3l);
    mpz_mul(qy, qy, p1);
    mpz_mod(qy, qy, curve.p);
    
    calFastAdd(&curve, px, py, one, qx, qy, one, &p0, &p1, &p2);
    calInv(p2, curve.p, &z);

    mpz_pow_ui(px, z, 2l);
    mpz_mul(px, px, p0);
    mpz_mod(px, px, curve.p);

    ret = (mpz_cmp(px, r) == 0 ? 1 : 0);

    ecFreeBasePoint(&curve);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(z);
    mpz_clear(h);
    mpz_clear(one);
    mpz_clear(p0);
    mpz_clear(p1);
    mpz_clear(p2);
    mpz_clear(tx);
    mpz_clear(ty);
    mpz_clear(px);
    mpz_clear(py);
    mpz_clear(qx);
    mpz_clear(qy);

    return ret;
}


int ecRecoverToPublicKey(uint8_t *content, uint32_t len, const char *sig_hex, char *pub_key_hex) {
    uint8_t v = sig_hex[128] == '0' ? (sig_hex[129] == '0' ? 27 : (sig_hex[129] == '1' ? 28 : 0)) : 0;
    if(!v) {
        return 0;
    }
    
    int ret = 0;
    mpz_t r, s, h;
    AlgCurve curve;
    char r_hex[65] = {'\0'}, s_hex[65] = {'\0'}, h_hex[len * 2 + 1];
    h_hex[len * 2] = '\0';

    memcpy(r_hex, sig_hex, 64);
    memcpy(s_hex, sig_hex+64, 64);
    calBytesToHex(content, len, h_hex);
    
    ecInitBasePoint(&curve);

    ret = mpz_init_set_str(r, r_hex, 16) == 0;
    ret = mpz_init_set_str(s, s_hex, 16) == 0;
    ret = mpz_init_set_str(h, h_hex, 16) == 0;

    if(!ret || mpz_cmp_ui(r, 0l) == 0 || mpz_cmp(r, curve.n) >= 0) {
        ecFreeBasePoint(&curve);
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(h);
        return 0;
    }
    if(mpz_cmp_ui(s, 0l) == 0 || mpz_cmp(s, curve.n) >= 0) {
        ecFreeBasePoint(&curve);
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(h);
        return 0;
    }
    mpz_t x, y, one, p0, p1, p2, q0, q1, q2, t0, t1, t2;
    mpz_init(x);
    mpz_init(y);
    mpz_init_set_ui(one, 1l);
    mpz_init(p0);
    mpz_init(p1);
    mpz_init(p2);
    mpz_init(q0);
    mpz_init(q1);
    mpz_init(q2);
    mpz_init(t0);
    mpz_init(t1);
    mpz_init(t2);

    mpz_mul(p0, r, curve.a);

    mpz_add(p0, p0, curve.b);
    mpz_pow_ui(x, r, 3l);
    mpz_add(x, x, p0);
    mpz_mod(x, x, curve.p);

    mpz_add_ui(p1, curve.p, 1l);
    mpz_fdiv_q_ui(p1, p1, 4);
    mpz_powm(y, x, p1, curve.p);
    if(!(mpz_odd_p(y) ^ (v%2))) {
        mpz_sub(y, curve.p, y);
    }

    mpz_pow_ui(p0, y, 2l);
    mpz_sub(p0, p0, x);
    mpz_mod(p0, p0, curve.p);
    if(mpz_cmp_ui(p0, 0l)) {
        ecFreeBasePoint(&curve);
        mpz_clear(r);
        mpz_clear(s);
        mpz_clear(h);
        mpz_clear(x);
        mpz_clear(y);
        mpz_clear(one);
        mpz_clear(p0);
        mpz_clear(p1);
        mpz_clear(p2);
        mpz_clear(q0);
        mpz_clear(q1);
        mpz_clear(q2);
        mpz_clear(t0);
        mpz_clear(t1);
        mpz_clear(t2);
        return 0;
    }

    mpz_sub(h, curve.n, h);
    mpz_mod(h, h, curve.n);
    calFastMul(&curve, curve.gx, curve.gy, one, h, &p0, &p1, &p2);
    calFastMul(&curve, r, y, one, s, &q0, &q1, &q2);

    calFastAdd(&curve, p0, p1, p2, q0, q1, q2, &t0, &t1, &t2);
    calInv(r, curve.n, &x);
    calFastMul(&curve, t0, t1, t2, x, &p0, &p1, &p2);
    calInv(p2, curve.p, &q0);

    mpz_pow_ui(x, q0, 2l);
    mpz_mul(x, x, p0);
    mpz_mod(x, x, curve.p);

    mpz_pow_ui(y, q0, 3l);
    mpz_mul(y, y, p1);
    mpz_mod(y, y, curve.p);
    
    char *x_hex = mpz_get_str(NULL, 16, x);
    char *y_hex = mpz_get_str(NULL, 16, y);
    unsigned int x_len = strlen(x_hex), y_len = strlen(y_hex);
    memset(pub_key_hex, '0', 128);
    memcpy(&pub_key_hex[64 - x_len], x_hex, x_len);
    memcpy(&pub_key_hex[128 - y_len], y_hex, y_len);

    ecFreeBasePoint(&curve);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(h);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(one);
    mpz_clear(p0);
    mpz_clear(p1);
    mpz_clear(p2);
    mpz_clear(q0);
    mpz_clear(q1);
    mpz_clear(q2);
    mpz_clear(t0);
    mpz_clear(t1);
    mpz_clear(t2);
    return 1;
}