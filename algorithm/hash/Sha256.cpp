/**
MIT License

Copyright (c) 2021 Archerxy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@Author archer 2021/3/3.
*/

#include "Sha256.h"

unsigned int rightRotate(unsigned int n, unsigned int d) {
    return (n >> d) | (n << (32-d));
}

const char* Sha256::hash(const char* data, const int len) {
    unsigned long h0 = 0x6a09e667;
    unsigned long h1 = 0xbb67ae85;
    unsigned long h2 = 0x3c6ef372;
    unsigned long h3 = 0xa54ff53a;
    unsigned long h4 = 0x510e527f;
    unsigned long h5 = 0x9b05688c;
    unsigned long h6 = 0x1f83d9ab;
    unsigned long h7 = 0x5be0cd19;

    unsigned long k[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
               0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
               0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
               0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
               0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
               0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
               0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
               0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


    unsigned int origLen = len;
    uint64_t origLenBits = origLen * 8;

    char* withOne = (char*) (malloc(origLen + 1));
    withOne[origLen] = 0x80;
    for (int i = 0; i < origLen; i++) {
        withOne[i] = data[i];
    }

    unsigned int newBitLen = (origLen+1) * 8;

    while (newBitLen % 512 != 448)
        newBitLen += 8;

    char* output = (char *)malloc((newBitLen/8 + 8));
    for (int i = 0; i < (newBitLen/8 + 8); i++) {
        output[i] = 0;
    }

    for (int i = 0; i < (origLen + 1); i++) {
        output[i] = *(withOne+i);
    }

    unsigned int outputLen = newBitLen/8 + 8;
    for (int i = 0; i < 8; i++) {
        output[outputLen -1 - i] = ((origLenBits >> (8 * i)) & 0xFF);
    }

    unsigned int num_chunks = outputLen * 8 / 512;

    for (int i = 0; i < num_chunks; i++) {
        unsigned long w[80] {};

        for(int j = 0; j < 16; j++) {
            w[j] =  ((output[i*512/8 + 4*j] << 24) & 0xFF000000) | ((output[i*512/8 + 4*j+1] << 16) & 0x00FF0000);
            w[j] |= ((output[i*512/8 + 4*j+2] << 8) & 0xFF00) | (output[i*512/8 + 4*j+3] & 0xFF);
        }

        for (int j = 16; j < 64; j++) {
            unsigned long s0 = rightRotate(w[j-15], 7) ^ rightRotate(w[j-15], 18) ^ (w[j-15] >> 3);
            unsigned long s1 = rightRotate(w[j-2], 17) ^ rightRotate(w[j-2], 19) ^ (w[j-2] >> 10);
            w[j] = w[j-16] + s0 + w[j-7] + s1;
        }

        unsigned long a = h0;
        unsigned long b = h1;
        unsigned long c = h2;
        unsigned long d = h3;
        unsigned long e = h4;
        unsigned long f = h5;
        unsigned long g = h6;
        unsigned long h = h7;

        for (int j = 0; j < 64; j++) {
            unsigned long S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
            unsigned long ch = (e & f) ^ (~e & g);
            unsigned long temp1 = h + S1 + ch + k[j] + w[j];
            unsigned long S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
            unsigned long maj = (a & b) ^ (a & c) ^ (b & c);
            unsigned long temp2 = S0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 = h0 + a;
        h1 = h1 + b;
        h2 = h2 + c;
        h3 = h3 + d;
        h4 = h4 + e;
        h5 = h5 + f;
        h6 = h6 + g;
        h7 = h7 + h;
    }

    char* hash = (char*) (malloc(32));
    for (int j = 0; j < 4; j++) {
        hash[j] = ((h0 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+4] = ((h1 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+8] = ((h2 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+12] = ((h3 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+16] = ((h4 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+20] = ((h5 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+24] = ((h6 >> 24-j*8) & 0xFF);
    }
    for (int j = 0; j < 4; j++) {
        hash[j+28] = ((h7 >> 24-j*8) & 0xFF);
    }
    return hash;
};