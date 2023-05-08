#include "alg.h"
#include "cal.h"

extern int calHexToBytes(const char *hex, unsigned int hex_len, uint8_t *bytes);
extern int calBytesToHex(uint8_t *bytes, unsigned int bytes_len, char *hex);
extern int sm3(uint8_t *content, uint32_t content_len, uint8_t *hash);

void sm2InitBasePoint(AlgCurve *curve) {
    mpz_init_set_str(curve->p,
        "115792089210356248756420345214020892766250353991924191454421193933289684991999", 10);
    mpz_init_set_str(curve->n,
        "115792089210356248756420345214020892766061623724957744567843809356293439045923", 10);
    mpz_init_set_str(curve->a, 
        "115792089210356248756420345214020892766250353991924191454421193933289684991996", 10);
    mpz_init_set_str(curve->b, 
        "18505919022281880113072981827955639221458448578012075254857346196103069175443", 10);
    mpz_init_set_str(curve->gx,
        "22963146547237050559479531362550074578802567295341616970375194840604139615431", 10);
    mpz_init_set_str(curve->gy,
        "85132369209828568825618990617112496413088388631904505083283536607588877201568", 10);
}

void sm2FreeBasePoint(AlgCurve *curve) {
    mpz_clear(curve->p);
    mpz_clear(curve->n);
    mpz_clear(curve->a);
    mpz_clear(curve->b);
    mpz_clear(curve->gx);
    mpz_clear(curve->gy);
}

int getUserIdBase(uint8_t *base_bytes) {
    // user_id("1234567812345678") + a + b + gx + gy
    // len = 146
    uint8_t base[] = 
    {
        0, 128, 49, 50, 51, 52, 53, 54, 55, 56, 49, 50, 51, 52, 
        53, 54, 55, 56, 255, 255, 255, 254, 255, 255, 255, 255, 
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
        255, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 252, 
        40, 233, 250, 158, 157, 159, 94, 52, 77, 90, 158, 75, 
        207, 101, 9, 167, 243, 151, 137, 245, 21, 171, 143, 146, 
        221, 188, 189, 65, 77, 148, 14, 147, 50, 196, 174, 44, 
        31, 25, 129, 25, 95, 153, 4, 70, 106, 57, 201, 148, 143, 
        227, 11, 191, 242, 102, 11, 225, 113, 90, 69, 137, 51, 
        76, 116, 199, 188, 55, 54, 162, 244, 246, 119, 156, 89, 
        189, 206, 227, 107, 105, 33, 83, 208, 169, 135, 124, 198, 
        42, 71, 64, 2, 223, 50, 229, 33, 57, 240, 160
    };
    memcpy(base_bytes, base, 146);
    return 1;
}

int sm2PrivateKeyToPublicKey(const char *priv_key_hex, char *pub_key_hex) {
    int ret = 0;
    AlgCurve curve;
    sm2InitBasePoint(&curve);

    mpz_t private_key_num;
    ret = mpz_init_set_str(private_key_num, priv_key_hex, 16) == 0;

    if(!ret || mpz_cmp(private_key_num, curve.n) > 0) {
        sm2FreeBasePoint(&curve);
        mpz_clear(private_key_num);
        return 0;
    }

    mpz_t x, y, t;
    mpz_init(x);
    mpz_init(y);
    mpz_init(t);

    calPointEc(&curve, private_key_num, &x, &y);

    char *x_hex = mpz_get_str(NULL, 16, x);
    char *y_hex = mpz_get_str(NULL, 16, y);
    unsigned int x_len = strlen(x_hex), y_len = strlen(y_hex);
    memset(pub_key_hex, '0', 128);
    memcpy(&pub_key_hex[64 - x_len], x_hex, x_len);
    memcpy(&pub_key_hex[128 - y_len], y_hex, y_len);

    sm2FreeBasePoint(&curve);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(t);
    return 1;
}

int sm2Sign(const char *priv_key_hex, uint8_t *content, uint32_t len, char *sig_hex) {
    int ret = 0;
    AlgCurve curve;
    sm2InitBasePoint(&curve);
    mpz_t private_key_num;
    ret = mpz_init_set_str(private_key_num, priv_key_hex, 16) == 0;
    if(!ret || mpz_cmp(private_key_num, curve.n) > 0) {
        sm2FreeBasePoint(&curve);
        mpz_clear(private_key_num);
        return 0;
    }
    
    mpz_t x, y, e;
    mpz_init(x);
    mpz_init(y);
    calPointEc(&curve, private_key_num, &x, &y);

    uint8_t base_bytes[146], x_bytes[32], y_bytes[32], hash[32];
    uint8_t hash_tmp[32+len], hash_input[210];
    char *x_hex = mpz_get_str(NULL, 16, x), *y_hex = mpz_get_str(NULL, 16, y);
    char hash_hex[65] = {'\0'};

    getUserIdBase(base_bytes);
    calHexToBytes(x_hex, strlen(x_hex), x_bytes);
    calHexToBytes(y_hex, strlen(y_hex), y_bytes);
    memcpy(hash_input, base_bytes, 146);
    memcpy(&hash_input[146], x_bytes, 32);
    memcpy(&hash_input[178], y_bytes, 32);
    sm3(hash_input, 210, hash_tmp);
    memcpy(&hash_tmp[32], content, len);
    sm3(hash_tmp, 32 + len, hash);
    calBytesToHex(hash, 32, hash_hex);
    mpz_init_set_str(e, hash_hex, 16);

    mpz_t k, r, s, tmp, da;
    mpz_init(k);
    mpz_init_set_ui(r, 0l);
    mpz_init_set_ui(s, 0l);
    mpz_init(tmp);
    mpz_init(da);

    while(mpz_cmp_ui(s, 0l) == 0) {
        calRandomK(&curve, &k);
        mpz_add(tmp, r, k);
        while(mpz_cmp_ui(r, 0l) == 0 || mpz_cmp(tmp, curve.n) == 0) {
            calPointEc(&curve, k, &x, &y);
            mpz_add(r, e, x);
            mpz_mod(r, r, curve.n);
        }
        mpz_mul(s, r, private_key_num);
        mpz_sub(s, k, s);
        mpz_mod(s, s, curve.n);

        mpz_add_ui(da, private_key_num, 1l);
        mpz_invert(da, da, curve.n);

        mpz_mul(s, s, da);
        mpz_mod(s, s, curve.n);
    }

    char *r_hex = mpz_get_str(NULL, 16, r);
    char *s_hex = mpz_get_str(NULL, 16, s);
    unsigned int r_len = strlen(r_hex), s_len = strlen(s_hex);
    memset(sig_hex, '0', 128);
    memcpy(&sig_hex[64 - r_len], r_hex, r_len);
    memcpy(&sig_hex[128 - s_len], s_hex, s_len);

    mpz_clear(private_key_num);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(e);
    mpz_clear(k);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(tmp);
    sm2FreeBasePoint(&curve);

    return 1;
}

int sm2Verify(const char *pub_key_hex, uint8_t *content, uint32_t len, const char *sig_hex) {
    int ret = 0;
    char x_hex[65] = {'\0'}, y_hex[65] = {'\0'};
    char r_hex[65] = {'\0'}, s_hex[65] = {'\0'};
    uint8_t base_bytes[146], x_bytes[32], y_bytes[32], hash[32];
    uint8_t hash_tmp[32+len], hash_input[210];
    char hash_hex[65] = {'\0'};

    memcpy(x_hex, pub_key_hex, 64);
    memcpy(y_hex, pub_key_hex+64, 64);
    memcpy(r_hex, sig_hex, 64);
    memcpy(s_hex, sig_hex+64, 64);
    
    if(!calHexToBytes(x_hex, strlen(x_hex), x_bytes)) {
        return 0;
    }
    if(!calHexToBytes(y_hex, strlen(y_hex), y_bytes)) {
        return 0;
    }

    AlgCurve curve;
    sm2InitBasePoint(&curve);
    mpz_t x, y, r, s;
    ret = mpz_init_set_str(x, x_hex, 16) == 0;
    ret = mpz_init_set_str(y, y_hex, 16) == 0;
    ret = mpz_init_set_str(r, r_hex, 16) == 0;
    ret = mpz_init_set_str(s, s_hex, 16) == 0;
    if(!ret) {
        return 0;
    }
    
    if(mpz_cmp(r, curve.n) == 0 || mpz_cmp(s, curve.n) == 0) {
        sm2FreeBasePoint(&curve);
        mpz_clear(x);
        mpz_clear(y);
        mpz_clear(r);
        mpz_clear(s);
        return 0;
    }

    getUserIdBase(base_bytes);
    memcpy(hash_input, base_bytes, 146);
    memcpy(&hash_input[146], x_bytes, 32);
    memcpy(&hash_input[178], y_bytes, 32);
    sm3(hash_input, 210, hash_tmp);
    memcpy(&hash_tmp[32], content, len);
    sm3(hash_tmp, 32 + len, hash);
    calBytesToHex(hash, 32, hash_hex);

    mpz_t e, one, p0, p1, p2, px, py, qx, qy, rs;
    mpz_init_set_str(e, hash_hex, 16);
    mpz_init_set_ui(one, 1l);
    mpz_init(p0);
    mpz_init(p1);
    mpz_init(p2);
    mpz_init(px);
    mpz_init(py);
    mpz_init(qx);
    mpz_init(qy);
    mpz_init(rs);

    calFastMul(&curve, curve.gx, curve.gy, one, s, &p0, &p1, &p2);
    calInv(p2, curve.p, &py);

    mpz_pow_ui(px, py, 2l);
    mpz_mul(px, px, p0);
    mpz_mod(px, px, curve.p);

    mpz_pow_ui(py, py, 3l);
    mpz_mul(py, py, p1);
    mpz_mod(py, py, curve.p);

    mpz_add(rs, r, s);
    mpz_mod(rs, rs, curve.n);
    calFastMul(&curve, x, y, one, rs, &p0, &p1, &p2);
    calInv(p2, curve.p, &qy);
    
    mpz_pow_ui(qx, qy, 2l);
    mpz_mul(qx, qx, p0);
    mpz_mod(qx, qx, curve.p);

    mpz_pow_ui(qy, qy, 3l);
    mpz_mul(qy, qy, p1);
    mpz_mod(qy, qy, curve.p);

    {
        printf("px = %s\n", mpz_get_str(NULL, 16, px));
        printf("py = %s\n", mpz_get_str(NULL, 16, py));
        printf("qx = %s\n", mpz_get_str(NULL, 16, qx));
        printf("qy = %s\n", mpz_get_str(NULL, 16, qy));
    }
    calFastAdd(&curve, px, py, one, qx, qy, one, &p0, &p1, &p2);
    calInv(p2, curve.p, &py);
    
    mpz_pow_ui(px, py, 2l);
    mpz_mul(px, px, p0);
    mpz_mod(px, px, curve.p);
    mpz_add(px, px, e);
    mpz_mod(px, px, curve.n);

    ret = mpz_cmp(px, r) == 0;

    sm2FreeBasePoint(&curve);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(e);
    mpz_clear(one);
    mpz_clear(p0);
    mpz_clear(p1);
    mpz_clear(p2);
    mpz_clear(px);
    mpz_clear(py);
    mpz_clear(qx);
    mpz_clear(qy);
    mpz_clear(rs);

    return ret;
}










