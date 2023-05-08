#include "alg.h"

#define SHA2_BLOCK_SIZE 64
#define SHA2_ROTL32(n, d) ((n >> d) | (n << (32-d)))
#define SHA2_ENDIAN(a) (((a&0xff)<<24)|(((a>>8)&0xff)<<16)|(((a>>16)&0xff)<<8)|((a>>24)&0xff))

void sha256Round(uint32_t *buf, uint32_t *hash) {
    uint32_t k[] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t s0, s1, S0, S1, t0, t1, ch, ma;
    for(int j = 0; j < 16; j++) {
        w[j] = SHA2_ENDIAN(buf[j]);
    }

    for (int j = 16; j < 64; j++) {
        s0 = SHA2_ROTL32(w[j-15], 7) ^ SHA2_ROTL32(w[j-15], 18) ^ (w[j-15] >> 3);
        s1 = SHA2_ROTL32(w[j-2], 17) ^ SHA2_ROTL32(w[j-2], 19) ^ (w[j-2] >> 10);
        w[j] = w[j-16] + s0 + w[j-7] + s1;
    }

    a = hash[0];
    b = hash[1];
    c = hash[2];
    d = hash[3];
    e = hash[4];
    f = hash[5];
    g = hash[6];
    h = hash[7];

    for (int j = 0; j < 64; j++) {
        S1 = SHA2_ROTL32(e, 6) ^ SHA2_ROTL32(e, 11) ^ SHA2_ROTL32(e, 25);
        ch = (e & f) ^ (~e & g);
        t0 = h + S1 + ch + k[j] + w[j];
        S0 = SHA2_ROTL32(a, 2) ^ SHA2_ROTL32(a, 13) ^ SHA2_ROTL32(a, 22);
        ma = (a & b) ^ (a & c) ^ (b & c);
        t1 = S0 + ma;

        h = g;
        g = f;
        f = e;
        e = d + t0;
        d = c;
        c = b;
        b = a;
        a = t0 + t1;
    }

    hash[0] = hash[0] + a;
    hash[1] = hash[1] + b;
    hash[2] = hash[2] + c;
    hash[3] = hash[3] + d;
    hash[4] = hash[4] + e;
    hash[5] = hash[5] + f;
    hash[6] = hash[6] + g;
    hash[7] = hash[7] + h;
}

int sha256(uint8_t *content, uint32_t content_len, uint8_t *hash) {
    uint32_t base[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint32_t buf[SHA2_BLOCK_SIZE];
    uint32_t count = 0, offset = 0;
    while(content_len >= SHA2_BLOCK_SIZE) {
        memcpy(buf, &content[offset], SHA2_BLOCK_SIZE);
        content_len -= SHA2_BLOCK_SIZE;
        offset += SHA2_BLOCK_SIZE;
        count++;

        sha256Round(buf, base);
    }
    uint32_t count_len = (content_len * 8) + count * (SHA2_BLOCK_SIZE * 8);
    memcpy(buf, &content[offset], content_len);
    memset(((uint8_t*)buf) + content_len, 0, SHA2_BLOCK_SIZE - content_len);

    ((uint8_t*)buf)[content_len] = 0x80;
    buf[SHA2_BLOCK_SIZE/4-1] = SHA2_ENDIAN(count_len);
    
    sha256Round(buf, base);

    for(int i = 0; i < 8; i++) {
        hash[i*4] = (base[i] >> 24)&0xff;
        hash[i*4+1] = (base[i] >> 16)&0xff;
        hash[i*4+2] = (base[i] >> 8)&0xff;
        hash[i*4+3] = base[i] & 0xff;
    }
    return 1;
}

int hmacSha256(uint8_t *key, uint32_t key_len, uint8_t *content, uint32_t content_len, uint8_t *hash) {
    uint8_t _5C[256], _36[256];
    uint8_t key_buf[64] = {0};
    uint8_t input0[64 + content_len], input1[64 + 32];
    for(int i = 0; i < 256; i++) {
        _5C[i] = i^0x5C;
        _36[i] = i^0x36;
    }
    if(key_len > 64) {
        sha256(key, key_len, key_buf);
    } else {
        memcpy(key_buf, key, key_len);
    }
    
    for(int i = 0; i < 64; ++i) {
        input0[i] = _36[key_buf[i]];
    }
    for(int i = 0; i < 64; ++i) {
        input1[i] = _5C[key_buf[i]];
    }

    memcpy(input0+64, content, content_len);
    sha256(input0, 64+content_len, input1+64);
    sha256(input1, 64+32, hash);
    return 1;
}

