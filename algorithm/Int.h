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

@Author archer 2021/3/9.
*/

#ifndef INT_H
#define INT_H

using namespace std;

#define ll long long
#define INT_NUMBER_RADIX 0x100000000ll
#define INT_NUMBER_MAX 0xffffffffll
#define INT_NUMBER_LEN 0x20
#define INT_NUMBER_OFFSET 0x20

#include <string>
#include <stdexcept>
#include <cstdlib>

class Int {
public:
    explicit Int(const char *str);

    explicit Int(const char *str, int radix);

    explicit Int(ll num);

    explicit Int();

    Int add(Int &num);

    Int sub(Int &num);

    Int mul(Int &num);

    Int div(Int &num);

    Int mod(Int &num);

    Int remain(Int &num);

    void doubleSelf();

    void half();

    Int pow(Int &num, Int &mod);

    Int pow(int num);

    bool equal(Int &num);

    int compareTo(Int &num);

    int isOdd();

    int isNeg();

    Int copy();

    Int operator+(Int num);

    Int operator-(Int num);

    Int operator*(Int num);

    Int operator/(Int num);

    Int operator%(Int num);

    Int operator+(ll num);

    Int operator-(ll num);

    Int operator*(ll num);

    Int operator/(ll num);

    Int operator%(ll num);

    string toString();

    string toString(int radix);

    const char * toBytes();

    static int isZero(Int &n);

    static int isOne(Int &n);

    static void bytesToHexStr(const char *bytes, const int bytesLen, string &outHexStr);

    static void hexStrToBytes(string &hex, char *outBytes);

private:
    ll m_mag[INT_NUMBER_LEN];

    int m_bitLen;

    bool m_isPos;

    void parseToMag(string str, int radix);

    explicit Int(const ll *mag, int bitLen, bool isPos);
};

#endif //INT_H
