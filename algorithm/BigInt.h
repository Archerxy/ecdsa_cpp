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

@Author archer 2021/3/8.
*/

#ifndef BIGINT_H
#define BIGINT_H

using namespace std;

#define BIGINT_NUMBER_RADIX 0x100000000ll
#define BIGINT_NUMBER_MAX 0xffffffffll
#define BIGINT_NUMBER_LEN 0x40
#define BIGINT_NUMBER_OFFSET 0x20
#define ll long long

#include <string>
#include <stdexcept>

class BigInt {
public:
    explicit BigInt(const char *str);

    explicit BigInt(const char *str, int radix);

    explicit BigInt(const ll num);

    explicit BigInt();

    BigInt add(BigInt &num);

    BigInt sub(BigInt &num);

    BigInt mul(BigInt &num);

    BigInt div(BigInt &num);

    BigInt mod(BigInt &num);

    BigInt remain(BigInt &num);

    void doubleSelf();

    void half();

    BigInt pow(BigInt &num, BigInt &mod);

    BigInt pow(int num);

    bool equal(BigInt &num);

    int compareTo(BigInt &num);

    int isOdd();

    int isNeg();

    BigInt copy();

    BigInt operator+(BigInt num);

    BigInt operator-(BigInt num);

    BigInt operator*(BigInt num);

    BigInt operator/(BigInt num);

    BigInt operator%(BigInt num);

    BigInt operator+(ll num);

    BigInt operator-(ll num);

    BigInt operator*(ll num);

    BigInt operator/(ll num);

    BigInt operator%(ll num);

    string toString();

    string toString(int radix);

    const char * toBytes();

    static int isZero(BigInt &n);

    static int isOne(BigInt &n);

    static void bytesToHexStr(const char *bytes, const int bytesLen, string &outHexStr);

    static void hexStrToBytes(string &hex, char *outBytes);

private:
    ll mag[BIGINT_NUMBER_LEN];

    int bitLen;

    void parseToMag(string str, int radix);

    explicit BigInt(const ll *mag_ptr, int len, bool mag_sign);
};
#define ONE BigInt(1ll)
#define TWO BigInt(2ll)
#define THREE BigInt(3ll)
#define FOUR BigInt(4ll)
#define FIVE BigInt(5ll)
#define SIX BigInt(6ll)
#define SEVEN BigInt(7ll)
#define EIGHT BigInt(8ll)
#define NINE BigInt(9ll)
#define TEN BigInt(10ll)
#define ZERO BigInt(0ll)

#endif //BIGINT_H
