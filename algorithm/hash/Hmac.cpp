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

#include "Hmac.h"

const unsigned char _36[] = {54, 55, 52, 53, 50, 51, 48, 49, 62, 63, 60, 61, 58, 59, 56, 57, 38, 39, 36, 37, 34, 35, 32, 33, 46, 47, 44, 45, 42, 43, 40, 41, 22, 23, 20, 21, 18, 19, 16, 17, 30, 31, 28, 29, 26, 27, 24, 25, 6, 7, 4, 5, 2, 3, 0, 1, 14, 15, 12, 13, 10, 11, 8, 9, 118, 119, 116, 117, 114, 115, 112, 113, 126, 127, 124, 125, 122, 123, 120, 121, 102, 103, 100, 101, 98, 99, 96, 97, 110, 111, 108, 109, 106, 107, 104, 105, 86, 87, 84, 85, 82, 83, 80, 81, 94, 95, 92, 93, 90, 91, 88, 89, 70, 71, 68, 69, 66, 67, 64, 65, 78, 79, 76, 77, 74, 75, 72, 73, 182, 183, 180, 181, 178, 179, 176, 177, 190, 191, 188, 189, 186, 187, 184, 185, 166, 167, 164, 165, 162, 163, 160, 161, 174, 175, 172, 173, 170, 171, 168, 169, 150, 151, 148, 149, 146, 147, 144, 145, 158, 159, 156, 157, 154, 155, 152, 153, 134, 135, 132, 133, 130, 131, 128, 129, 142, 143, 140, 141, 138, 139, 136, 137, 246, 247, 244, 245, 242, 243, 240, 241, 254, 255, 252, 253, 250, 251, 248, 249, 230, 231, 228, 229, 226, 227, 224, 225, 238, 239, 236, 237, 234, 235, 232, 233, 214, 215, 212, 213, 210, 211, 208, 209, 222, 223, 220, 221, 218, 219, 216, 217, 198, 199, 196, 197, 194, 195, 192, 193, 206, 207, 204, 205, 202, 203, 200, 201};
const unsigned char _5C[] = {92, 93, 94, 95, 88, 89, 90, 91, 84, 85, 86, 87, 80, 81, 82, 83, 76, 77, 78, 79, 72, 73, 74, 75, 68, 69, 70, 71, 64, 65, 66, 67, 124, 125, 126, 127, 120, 121, 122, 123, 116, 117, 118, 119, 112, 113, 114, 115, 108, 109, 110, 111, 104, 105, 106, 107, 100, 101, 102, 103, 96, 97, 98, 99, 28, 29, 30, 31, 24, 25, 26, 27, 20, 21, 22, 23, 16, 17, 18, 19, 12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3, 60, 61, 62, 63, 56, 57, 58, 59, 52, 53, 54, 55, 48, 49, 50, 51, 44, 45, 46, 47, 40, 41, 42, 43, 36, 37, 38, 39, 32, 33, 34, 35, 220, 221, 222, 223, 216, 217, 218, 219, 212, 213, 214, 215, 208, 209, 210, 211, 204, 205, 206, 207, 200, 201, 202, 203, 196, 197, 198, 199, 192, 193, 194, 195, 252, 253, 254, 255, 248, 249, 250, 251, 244, 245, 246, 247, 240, 241, 242, 243, 236, 237, 238, 239, 232, 233, 234, 235, 228, 229, 230, 231, 224, 225, 226, 227, 156, 157, 158, 159, 152, 153, 154, 155, 148, 149, 150, 151, 144, 145, 146, 147, 140, 141, 142, 143, 136, 137, 138, 139, 132, 133, 134, 135, 128, 129, 130, 131, 188, 189, 190, 191, 184, 185, 186, 187, 180, 181, 182, 183, 176, 177, 178, 179, 172, 173, 174, 175, 168, 169, 170, 171, 164, 165, 166, 167, 160, 161, 162, 163};

void translate(const char* bs, char* out, const unsigned char* trans, const int len) {
    for(int i = 0; i < len; ++i) {
        int k = bs[i];
        if(k < 0)
            k += 256;
        out[i] = trans[k];
    }
}

const char* Hmac::sha256(const char *key, const int keyLen, const char *data, const int dataLen) {
    char key64[64]{};
    int last = keyLen<64?keyLen:64;
    for(int i = 0; i < last; ++i) {
        key64[i] = key[i];
    }
    char* innerInput = static_cast<char *>(malloc(sizeof(char) * (64+dataLen)));
    translate(key64, innerInput, _36, 64);
    for(int i = 0; i < dataLen; ++i)
        innerInput[i+64] = data[i];
    const char* innerOutput = Sha256::hash(innerInput, 64+dataLen);

    char* outerInput = static_cast<char *>(malloc(sizeof(char) * 96));
    translate(key64, outerInput, _5C, 64);
    for(int i = 0; i < 32; ++i)
        outerInput[i+64] = innerOutput[i];

    free(innerInput);
    free(outerInput);
    return Sha256::hash(outerInput, 96);
}