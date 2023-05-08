#include "alg.h"

#define SHA3_BLOCK_SIZE 136
#define I64(x) x##LL
#define SHA3_ROTL64(q, k) ((q << k) ^ (q >> (64 - k)))

void keccak256Round(uint64_t *buf, uint64_t *hash) {
    uint8_t constant[]  = {
        1, 26, 94, 112, 31, 33, 121, 85, 14, 12, 53, 38, 63, 79, 93, 83, 82, 72, 22, 102, 121, 88, 33, 116,
        1, 6, 9, 22, 14, 20, 2, 12, 13, 19, 23, 15, 4, 24, 21, 8, 16, 5, 3, 18, 17, 11, 7, 10,
        1, 62, 28, 27, 36, 44, 6, 55, 20, 3, 10, 43, 25, 39, 41, 45, 15, 21, 8, 18, 2, 61, 56, 14,
    };
    
    for(int i = 0; i < SHA3_BLOCK_SIZE/8; i++) {
        hash[i] ^= buf[i];
    }

    uint64_t C[5], D[5], hash0, hash1;
    for(int r = 0; r < 24; r++) {
        for (int i = 0; i < 5; i++) {
            C[i] = hash[i];
            for (int j = 5; j < 21; j += 5) { 
                C[i] ^= hash[i + j]; 
            }
        }
        for (int i = 0; i < 5; i++) {
            D[i] = SHA3_ROTL64(C[(i + 1) % 5], 1) ^ C[(i + 4) % 5];
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 21; j += 5) { 
                hash[i + j] ^= D[i]; 
            }
        }
        for(int i = 1; i < 25; i++) {
            hash[i] = SHA3_ROTL64(hash[i], constant[48 + i - 1]);
        }

        hash1 = hash[1];
        for (int i = 1; i < 24; i++) {
            hash[constant[24 + i - 1]] = hash[constant[24 + i]];
        }
        hash[10] = hash1;

        
        for (int i = 0; i < 21; i += 5) {
            hash0 = hash[0 + i];
            hash1 = hash[1 + i];

            hash[0 + i] ^= ~hash1 & hash[2 + i];
            hash[1 + i] ^= ~hash[2 + i] & hash[3 + i];
            hash[2 + i] ^= ~hash[3 + i] & hash[4 + i];
            hash[3 + i] ^= ~hash[4 + i] & hash0;
            hash[4 + i] ^= ~hash0 & hash1;
        }

        hash0 = 0;
        if (constant[r] & (1 << 6)) { 
            hash0 |= ((uint64_t)1 << 63); 
        }
        if (constant[r] & (1 << 5)) { 
            hash0 |= ((uint64_t)1 << 31); 
        }
        if (constant[r] & (1 << 4)) { 
            hash0 |= ((uint64_t)1 << 15); 
        }
        if (constant[r] & (1 << 3)) { 
            hash0 |= ((uint64_t)1 << 7); 
        }
        if (constant[r] & (1 << 2)) { 
            hash0 |= ((uint64_t)1 << 3); 
        }
        if (constant[r] & (1 << 1)) { 
            hash0 |= ((uint64_t)1 << 1); 
        }
        if (constant[r] & (1 << 0)) { 
            hash0 |= ((uint64_t)1 << 0); 
        }
        hash[0] ^= hash0;
    }
}

int keccak256(uint8_t *content, uint32_t content_len, uint8_t *hash) {
    uint64_t h[25] = {0}, buf[SHA3_BLOCK_SIZE/8];
    uint32_t offset = 0, output_len = 32;
    while(content_len >= SHA3_BLOCK_SIZE) {
        memcpy(buf, &content[offset], SHA3_BLOCK_SIZE);
        content_len -= SHA3_BLOCK_SIZE;
        offset += SHA3_BLOCK_SIZE;
        keccak256Round(buf, h);
    }
    if(content_len > 0) {
        memcpy(buf, &content[offset], content_len);
        memset(((uint8_t*)buf) + content_len, 0, SHA3_BLOCK_SIZE - content_len);
        ((uint8_t*)buf)[content_len] |= 0x01;
        ((uint8_t*)buf)[SHA3_BLOCK_SIZE - 1] |= 0x80;
        keccak256Round(buf, h);
    }
    memcpy(hash, h, output_len);
    return 1;
}