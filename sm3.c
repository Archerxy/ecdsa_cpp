#include "alg.h"

#define SM3_BLOCK_SIZE 64

#define SM3_ROTL(x,n) ((x<<n)|(x>>(32-n)))
#define FF1(a,b,c) ((a)^(b)^(c))
#define FF2(a,b,c) (((a)&(b))|((a)&(c))|((b)&(c)))
#define GG1(a,b,c) ((a)^(b)^(c))
#define GG2(a,b,c) (((a)&(b))|((~a)&(c)))
#define P1(a) (a^SM3_ROTL(a,9)^SM3_ROTL(a,17))
#define P2(a) (a^SM3_ROTL(a,15)^SM3_ROTL(a,23))
#define SM3_ENDIAN32(a) (((a&0xff)<<24)|(((a>>8)&0xff)<<16)|(((a>>16)&0xff)<<8)|((a>>24)&0xff))


int sm3CF(uint32_t *in, uint32_t *hash) {
    uint32_t W[68], W1[64];
    uint32_t a = hash[0], b = hash[1], c = hash[2], d = hash[3];
    uint32_t e = hash[4], f = hash[5], g = hash[6], h = hash[7];
    uint32_t ss1, ss2, tt1, tt2;
    uint32_t T[64];
    for(int i = 0; i < 16; i++) {
        T[i] = 0x79cc4519;
    }
    for(int i = 16; i < 64; i++) {
        T[i] = 0x7a879d8a;
    }

    for(int i = 0; i < 16; i++) {
        W[i] = SM3_ENDIAN32(in[i]);;
    }
    for(int i = 16; i < 68; i++) {
        W[i] = P2((W[i-16] ^ W[i-9] ^ SM3_ROTL(W[i-3], (uint32_t)15))) ^ SM3_ROTL(W[i-13], (uint32_t)7) ^ W[i-6];
    }
    for(int i = 0; i < 64; i++) {
        W1[i] = W[i] ^ W[i + 4];
    }
    for(int i = 0; i < 64; i++) {
        ss1 = SM3_ROTL((SM3_ROTL(a, (uint32_t)12) + e + SM3_ROTL(T[i], (uint32_t)i)), (uint32_t)7);
        ss2 = ss1 ^ SM3_ROTL(a, (uint32_t)12);
        if(i >= 0 && i <= 15) {
            tt1 = FF1(a, b, c) + d + ss2 + W1[i];
            tt2 = GG1(e, f, g) + h + ss1 + W[i];
        } else {
            tt1 = FF2(a, b, c) + d + ss2 + W1[i];
            tt2 = GG2(e, f, g) + h + ss1 + W[i];
        }

        d = c;
        c = SM3_ROTL(b, (uint32_t)9);
        b = a;
        a = tt1;
        h = g;
        g = SM3_ROTL(f, (uint32_t)19);
        f = e;
        e = P1(tt2);
    }

    hash[0] = a ^ hash[0];
    hash[1] = b ^ hash[1];
    hash[2] = c ^ hash[2];
    hash[3] = d ^ hash[3];
    hash[4] = e ^ hash[4];
    hash[5] = f ^ hash[5];
    hash[6] = g ^ hash[6];
    hash[7] = h ^ hash[7];

    return 1;
}

int sm3(uint8_t *content, uint32_t content_len, uint8_t *hash) {
    uint32_t hash_base[] = {0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 
                0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e};
    uint32_t buf[SM3_BLOCK_SIZE/4];
    uint32_t count = 0, offset = 0;
    while(content_len >= SM3_BLOCK_SIZE) {
        memcpy(buf, &content[offset], SM3_BLOCK_SIZE);
        content_len -= SM3_BLOCK_SIZE;
        offset += SM3_BLOCK_SIZE;
        count++;

        sm3CF(buf, hash_base);
    }
    uint32_t count_len = (content_len * 8) + count * (SM3_BLOCK_SIZE * 8);
    memcpy(buf, &content[offset], content_len);
    memset(((uint8_t*)buf) + content_len, 0, SM3_BLOCK_SIZE - content_len);

    ((uint8_t*)buf)[content_len] = 0x80;
    buf[SM3_BLOCK_SIZE/4-1] = SM3_ENDIAN32(count_len);
    
    sm3CF(buf, hash_base);

    for(int i = 0; i < 8; i++) {
        hash[i*4] = (hash_base[i] >> 24)&0xff;
        hash[i*4+1] = (hash_base[i] >> 16)&0xff;
        hash[i*4+2] = (hash_base[i] >> 8)&0xff;
        hash[i*4+3] = hash_base[i] & 0xff;
    }
    return 1;
}
