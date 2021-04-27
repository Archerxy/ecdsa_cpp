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


#include "BigInt.h"

static char intToHex[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static int  hexToInt[] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          127, 127, 127, 127, 127, 127, 127, 127, 127,
                          0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                          127, 127, 127, 127, 127, 127, 127,
                          10, 11, 12, 13, 14, 15,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          10, 11, 12, 13, 14, 15,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
                          127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};

static int lenPerRadix[] = {0, 0, 30, 19, 15, 13, 11, 11, 10, 9, 9, 8, 8, 8, 8, 7, 7};
static long intRadix[] = {0, 0,
                          0x40000000, 0x4546b3db, 0x40000000, 0x48c27395, 0x159fd800,
                          0x75db9c97, 0x40000000, 0x17179149, 0x3b9aca00, 0xcc6db61,
                          0x19a10000, 0x309f1021, 0x57f6c100, 0xa2f1b6f,  0x10000000};

bool bigint_number_check_char(char c) {
    return (48 <= c && c <= 57)||(65 <= c && c <= 70)||(97 <= c && c <= 102);
}

static ll bigint_number_parse_to_long(string &s, int radix) {
    int i = (int)s.length()-1;
    ll r = 1, sum = 0;
    while(i >= 0) {
        if(!bigint_number_check_char(s[i]))
            throw runtime_error("Invalid number, "+s);

        sum += r*hexToInt[s[i]];
        r *= radix;
        --i;
    }
    return sum;
}
int BigInt::isZero(BigInt &n) {
    return n.bitLen == 0;
}

int BigInt::isOne(BigInt &n) {
    return (n.bitLen==1)&&(n.mag[BIGINT_NUMBER_LEN-1] == 1);
}

void BigInt::bytesToHexStr(const char *bytes, const int bytesLen, string &outHexStr) {
    outHexStr = "";
    for(int i = 0; i < bytesLen; ++i) {
        int k = bytes[i];
        if(k < 0)
            k += 256;
        outHexStr += intToHex[k>>4];
        outHexStr += intToHex[k&0xf];
    }
}

void BigInt::hexStrToBytes(string &hex, char *outBytes) {
    int l = hex.length();
    if((l & 1) == 1)
        hex = "0" + hex;
    int len = (l + 1) >> 1;
    for(int i = 0; i < len; ++i) {
        char b0 = hex[i*2], b1 = hex[i*2+1];
        if(b0 < 0 || b1 < 0)
            throw runtime_error("Invalid hex string. " + hex);
        outBytes[i] = (char)(((hexToInt[b0]<<4)&0xff)|(hexToInt[b1]&0xf));
    }
}

int BigInt::isOdd() {
    if(mag[0] == BIGINT_NUMBER_MAX)
        return (mag[BIGINT_NUMBER_LEN-1]&1)==0;
    return (mag[BIGINT_NUMBER_LEN-1]&1)==1;
}

int BigInt::isNeg() {
    return mag[0] == BIGINT_NUMBER_MAX;
}

BigInt BigInt::add(BigInt &num) {
    ll new_mag[BIGINT_NUMBER_LEN];

    ll *m_mag = mag, *n_mag = num.mag;

    ll m, n;

    short int add = 0;
    for(int i = BIGINT_NUMBER_LEN-1; i >= 0; --i) {
        new_mag[i] = 0;

        m = m_mag[i];
        n = n_mag[i];
        if(m + n + add >= BIGINT_NUMBER_RADIX) {
            new_mag[i] = m + n + add - BIGINT_NUMBER_RADIX;
            add = 1;
        } else {
            new_mag[i] = m + n + add;
            add = 0;
        }
    }
    if(add == 1) {
        for(int i = BIGINT_NUMBER_LEN-1; i >= 0; --i) {
            if(new_mag[i]+add >= BIGINT_NUMBER_RADIX)
                new_mag[i] = 0;
            else {
                ++new_mag[i];
                break;
            }
        }
    }
    return BigInt(new_mag, BIGINT_NUMBER_LEN, new_mag[0] == 0);
}

BigInt BigInt::sub(BigInt &num) {
    if(compareTo(num) == 0)
        return BigInt(0ll);
    ll new_mag[BIGINT_NUMBER_LEN];
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i)
        new_mag[i] = BIGINT_NUMBER_MAX - num.mag[i];
    BigInt neg(new_mag, BIGINT_NUMBER_LEN, new_mag[0] == 0);
    BigInt newInt = add(neg);
    return newInt;
}

void bigint_number_quick_mul(ll* n, int n_l, ll* m, int m_l) {
    int index;
    unsigned ll mul, add = 0;
    ll t[BIGINT_NUMBER_LEN];
    for(int i = BIGINT_NUMBER_LEN -1; i >= BIGINT_NUMBER_LEN - n_l - 1; --i) {
        t[i] = n[i];
        n[i] = 0;
    }
    for(int i = 0; i <= n_l; ++i) {
        for(int j = 0; j <= m_l; ++j) {
            index = BIGINT_NUMBER_LEN - i - j - 1;

            mul = t[BIGINT_NUMBER_LEN-i-1]*m[BIGINT_NUMBER_LEN-j-1];
            n[index] = (mul&BIGINT_NUMBER_MAX) + n[index] + add;
            add = (mul>>BIGINT_NUMBER_OFFSET) + (n[index]>>BIGINT_NUMBER_OFFSET);
            n[index] = n[index]&BIGINT_NUMBER_MAX;
        }
    }
}

BigInt BigInt::mul(BigInt &num) {
    if(isZero(num))
        return BigInt(0ll);
    if (isOne(num))
        return BigInt(mag, BIGINT_NUMBER_LEN, mag[0] == 0);

    ll n[BIGINT_NUMBER_LEN], m[BIGINT_NUMBER_LEN];
    int n_sign = 1, m_sign = 1;
    if (mag[0] == BIGINT_NUMBER_MAX)
        n_sign = -1;
    if (num.mag[0] == BIGINT_NUMBER_MAX)
        m_sign = -1;

    for (int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        n[i] = (n_sign < 0) ? BIGINT_NUMBER_MAX - mag[i] : mag[i];
        m[i] = (m_sign < 0) ? BIGINT_NUMBER_MAX - num.mag[i] : num.mag[i];
    }

    bigint_number_quick_mul(n, bitLen, m, num.bitLen);
    if (m_sign * n_sign < 0) {
        for (ll &i : n)
            i = BIGINT_NUMBER_MAX - i;
    }
    return BigInt(n, BIGINT_NUMBER_LEN, n[0] == 0);
}

void bigint_number_get_remain(ll *n, int n_s, int n_e, ll *m, int m_s, int m_e, ll s, ll e, ll *div) {
    ll mul = (s+e)>>1;
    if(s >= e - 1) {
        int l = n_e - n_s, ans_off = (n_e - n_s == m_e - m_s)?0:-1, add = 0;
        for(int i = l; i >= 0; --i) {
            if(n[i+n_s] + add < m[i+m_s+ans_off]) {
                n[i+n_s] = BIGINT_NUMBER_RADIX + n[i+n_s] + add - m[i+m_s+ans_off];
                add = -1;
            } else {
                n[i+n_s] = n[i+n_s] + add - m[i+m_s+ans_off];
                add = 0;
            }
        }
        if(div)
            *div = mul;
        return ;
    }
    unsigned ll mv = mul*m[m_s], nv;
    if(n_e - n_s == m_e - m_s) {
        nv = n[n_s];
    } else {
        nv = ((unsigned ll )n[n_s]*BIGINT_NUMBER_RADIX) + n[n_s+1];
    }
    if(mv > nv) {
        bigint_number_get_remain(n, n_s, n_e, m, m_s, m_e, s, mul, div);
        return ;
    }
    if(mv + m[m_s] < nv - mul) {
        bigint_number_get_remain(n, n_s, n_e, m, m_s, m_e, mul, e, div);
        return;
    }

    ll mul_ans[BIGINT_NUMBER_LEN] {};
    unsigned ll sum, add = 0;
    for(int i = m_e; i >= m_s -1; --i) {
        sum = m[i]*mul + add;
        add = (sum>>BIGINT_NUMBER_OFFSET);
        mul_ans[i] = sum&BIGINT_NUMBER_MAX;
    }

    int l = n_e - n_s, ans_off = (n_e - n_s == m_e - m_s)?0:-1;
    ll tmp_remain[BIGINT_NUMBER_LEN]{}, borrow = 0;
    for(int i = l; i >= 0; --i) {
        if(n[i+n_s] + borrow < mul_ans[i+m_s+ans_off]) {
            tmp_remain[i+n_s] = BIGINT_NUMBER_RADIX + n[i+n_s] + borrow - mul_ans[i+m_s+ans_off];
            borrow = -1;
        } else {
            tmp_remain[i+n_s] = n[i+n_s] + borrow - mul_ans[i+m_s+ans_off];
            borrow = 0;
        }
    }
    if(borrow < 0) {
        bigint_number_get_remain(n, n_s, n_e, m, m_s, m_e, s, mul, div);
        return ;
    }
    borrow = 0;
    for(int i = 0; i<= l; ++i) {
        if(tmp_remain[i+n_s] < m[i+m_s+ans_off]) {
            borrow = -1;
            break;
        }
        if(tmp_remain[i+n_s] > m[i+m_s+ans_off]) {
            borrow = 1;
            break;
        }
    }
    if(borrow == 0) {
        if(div)
            *div = mul;
        for(int i = n_s; i <= n_e; ++i)
            n[i] = 0;
        return ;
    }
    if(borrow > 0) {
        bigint_number_get_remain(n, n_s, n_e, m, m_s, m_e, mul, e, div);
        return ;
    }
    if(div)
        *div = mul;
    for(int i = n_s; i <= n_e; ++i)
        n[i] = tmp_remain[i];
}

void bigint_number_divide_and_remain(const ll *n, int n_l, ll *m, int m_l, ll *div, ll *remain) {
    int n_s = BIGINT_NUMBER_LEN-n_l, n_e = n_s+m_l-1, m_s = BIGINT_NUMBER_LEN-m_l, m_e = BIGINT_NUMBER_LEN-1;
    int remainFlag = 1;
    if(!remain) {
        remain = static_cast<ll *>(malloc(sizeof(ll) * BIGINT_NUMBER_LEN));
        remainFlag = 0;
    }
    for(int i = n_s; i < BIGINT_NUMBER_LEN; ++i) {
        remain[i] = n[i];
    }
    ll *mul = static_cast<ll *>(malloc(sizeof(ll)));
    while(n_e < BIGINT_NUMBER_LEN) {
        if(n_e-n_s == m_e-m_s) {
            for(int i = 0; i < m_l; ++i) {
                if(n[i+n_s] > m[i+m_s])
                    break;
                if(n[i+n_s] < m[i+m_s]) {
                    ++n_e;
                    break;
                }
            }
        }
        if(n_e >= BIGINT_NUMBER_LEN)
            break;

        bigint_number_get_remain(remain, n_s, n_e, m, m_s, m_e, 0, BIGINT_NUMBER_RADIX, mul);
        if(div)
            div[n_e] = *mul;
        while(remain[n_s] == 0)
            ++n_s;
        ++n_e;
    }
    if(!remainFlag) {
        free(remain);
    }
    free(mul);
}

void bigint_number_one_bit_div(const ll *n, int n_l, ll mul, ll* div) {
    int min = BIGINT_NUMBER_LEN-n_l;
    unsigned ll sum, mod = 0;
    for(int i = min; i < BIGINT_NUMBER_LEN; ++i) {
        sum = n[i] + mod*BIGINT_NUMBER_RADIX;
        div[i] = sum/mul;
        mod = sum%mul;
    }
}

BigInt BigInt::div(BigInt &num) {
    if(isZero(num)) {
        throw range_error("Divided by zero.");
    }
    if(isOne(num))
        return BigInt(mag, BIGINT_NUMBER_LEN, mag[0] == 0);
    ll n[BIGINT_NUMBER_LEN], m[BIGINT_NUMBER_LEN];
    int n_sign = (mag[0] == BIGINT_NUMBER_MAX)?-1:1, m_sign = (num.mag[0] == BIGINT_NUMBER_MAX)?-1:1, off = 0;
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        n[i] = (mag[0] == BIGINT_NUMBER_MAX)?BIGINT_NUMBER_MAX-mag[i]:mag[i];
        m[i] = (num.mag[0] == BIGINT_NUMBER_MAX)?BIGINT_NUMBER_MAX-num.mag[i]:num.mag[i];
        if(off == 0 && n[i] > m[i])
            off = 1;
        if(off == 0 && n[i] < m[i])
            off = -1;
    }
    if(off < 0)
        return BigInt(0ll);
    if(off == 0)
        return BigInt(1ll);

    int n_l = bitLen, m_l = num.bitLen;
    ll div[BIGINT_NUMBER_LEN] {};
    if(m_l == 1) {
        bigint_number_one_bit_div(n, n_l, m[BIGINT_NUMBER_LEN-1], div);
    } else {
        bigint_number_divide_and_remain(n, n_l, m, m_l, div, NULL);
        if(m_sign*n_sign < 0)
            for(ll &i : div)
                i = BIGINT_NUMBER_MAX - i;
    }
    return BigInt(div, BIGINT_NUMBER_LEN, m_sign*n_sign > 0);
}

ll bigint_number_oneBitMod(ll *n, int n_l, ll mul) {
    int min = BIGINT_NUMBER_LEN-n_l;
    unsigned ll sum, mod = 0;
    for(int i = min; i < BIGINT_NUMBER_LEN; ++i) {
        sum = n[i] + mod*BIGINT_NUMBER_RADIX;
        mod = sum%mul;
    }
    return mod;
}

BigInt BigInt::remain(BigInt &num) {
    if (isZero(num)) {
        throw range_error("Divided by zero in mod calculate.");
    }
    if (num.mag[0] == BIGINT_NUMBER_MAX) {
        throw range_error("Negative number in mod calculate.");
    }
    if(isOne(num))
        return BigInt(0ll);
    int n_sign = 1, off = 0;
    if(mag[0] == BIGINT_NUMBER_MAX)
        n_sign = -1;
    ll n[BIGINT_NUMBER_LEN], m[BIGINT_NUMBER_LEN];
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        n[i] = (mag[0] == BIGINT_NUMBER_MAX)?BIGINT_NUMBER_MAX-mag[i]:mag[i];
        m[i] = num.mag[i];
        if(off == 0 && n[i] > m[i])
            off = 1;
        if(off == 0 && n[i] < m[i])
            off = -1;
    }

    if(off < 0)
        return BigInt(mag, BIGINT_NUMBER_LEN, mag[0] == 0);
    if(off == 0)
        return BigInt(0ll);

    int n_l = bitLen, m_l = num.bitLen;
    if(m_l == 1) {
        ll mod = bigint_number_oneBitMod(n, bitLen, m[BIGINT_NUMBER_LEN-1]);
        return BigInt(mod*n_sign);
    } else {
        ll mod[BIGINT_NUMBER_LEN] {};
        bigint_number_divide_and_remain(n, n_l, m, m_l, NULL, mod);
        if(n_sign < 0)
            for(ll &i : mod)
                i = BIGINT_NUMBER_MAX - i;
        return BigInt(mod, BIGINT_NUMBER_LEN, mod[0] == 0);
    }
}

BigInt BigInt::mod(BigInt &num) {
    BigInt rest = remain(num);
    if(rest.mag[0] == BIGINT_NUMBER_MAX) {
        unsigned ll sum, add = 1;
        for(int i = BIGINT_NUMBER_LEN-1; i >= 0; --i) {
            sum = num.mag[i] + add + rest.mag[i];
            if(sum >= BIGINT_NUMBER_RADIX) {
                rest.mag[i] = sum - BIGINT_NUMBER_RADIX;
                add = 1;
            } else {
                rest.mag[i] = sum;
                add = 0;
            }
        }
        int l = BIGINT_NUMBER_LEN - num.bitLen - 1;
        while(rest.mag[l] == 0)
            l++;
        rest.bitLen = BIGINT_NUMBER_LEN - l;
    }
    return rest;
}

void BigInt::half() {
    unsigned ll add = 0, m;
    bool isNeg = (mag[0] == BIGINT_NUMBER_MAX);
    int min = BIGINT_NUMBER_LEN-bitLen;
    for(int i = min; i < BIGINT_NUMBER_LEN; ++i) {
        if(isNeg)
            mag[i] = BIGINT_NUMBER_MAX - mag[i];
        m = mag[i] + add*BIGINT_NUMBER_RADIX;
        add = m&1;
        mag[i] = m>>1;
        if(isNeg)
            mag[i] = BIGINT_NUMBER_MAX - mag[i];
    }
    if(mag[BIGINT_NUMBER_LEN-bitLen] == 0)
        --bitLen;
}

void BigInt::doubleSelf() {
    unsigned ll m, add = 0;
    bool isNeg = (mag[0] == BIGINT_NUMBER_MAX);
    int min = (bitLen == BIGINT_NUMBER_LEN)?0:BIGINT_NUMBER_LEN-bitLen-1;
    for(int i = BIGINT_NUMBER_LEN - 1; i >= min; --i) {
        if(isNeg)
            mag[i] = BIGINT_NUMBER_MAX - mag[i];
        m = (mag[i]<<1) + add;
        mag[i] = m&BIGINT_NUMBER_MAX;
        add = m>>BIGINT_NUMBER_OFFSET;
    }
    if(mag[BIGINT_NUMBER_LEN-bitLen-1] > 0)
        ++bitLen;
}

int bigint_number_quick_square(ll *n, int n_l) {
    int index;
    unsigned ll mul, add = 0;
    ll m[BIGINT_NUMBER_LEN];
    for(int i = BIGINT_NUMBER_LEN - 1; i >= BIGINT_NUMBER_LEN - n_l - 1; --i) {
        m[i] = n[i];
        n[i] = 0;
    }
    for(int i = 0; i <= n_l; ++i) {
        for(int j = 0; j <= n_l; ++j) {
            index = BIGINT_NUMBER_LEN - i - j - 1;

            mul = m[BIGINT_NUMBER_LEN-i-1]*m[BIGINT_NUMBER_LEN-j-1];
            n[index] = (mul&BIGINT_NUMBER_MAX) + n[index] + add;
            add = (mul>>BIGINT_NUMBER_OFFSET) + (n[index]>>BIGINT_NUMBER_OFFSET);
            n[index] = n[index]&BIGINT_NUMBER_MAX;
        }
    }
    index = BIGINT_NUMBER_LEN - 2 * n_l - 1;
    while(n[index] == 0)
        ++index;
    return BIGINT_NUMBER_LEN - index;
}

int bigint_number_binary_pow(ll *n, int n_l, ll *m, int m_l, ll *mod, int mod_l, ll *ans) {
    if(m_l == 0) {
        ans[BIGINT_NUMBER_LEN-1] = 1;
        return 1;
    }
    if(m_l == 1 && m[BIGINT_NUMBER_LEN-1] == 1) {
        for(int i = BIGINT_NUMBER_LEN - n_l; i < BIGINT_NUMBER_LEN; ++i)
            ans[i] = n[i];
        return n_l;
    }
    /*
     * quick pow here.
     * */
    int isOdd = m[BIGINT_NUMBER_LEN-1]&1;
    unsigned ll add = 0, sum;
    for(int i = BIGINT_NUMBER_LEN-m_l; i < BIGINT_NUMBER_LEN; ++i) {
        sum = m[i] + add*BIGINT_NUMBER_RADIX;
        add = sum&1;
        m[i] = sum>>1;
    }
    if(m[BIGINT_NUMBER_LEN-m_l] == 0)
        --m_l;
    int ans_l = bigint_number_binary_pow(n, n_l, m, m_l, mod, mod_l, ans);

    ans_l = bigint_number_quick_square(ans, ans_l);

    if(isOdd) {
        bigint_number_quick_mul(ans, ans_l, n, n_l);

        int index = BIGINT_NUMBER_LEN - ans_l - n_l - 1;
        while(ans[index] == 0)
            ++index;
        ans_l = BIGINT_NUMBER_LEN-index;
    }
    /*
     * quick mod here
     * */
    if(mod_l == 1) {
        ll final = bigint_number_oneBitMod(ans, ans_l, mod[BIGINT_NUMBER_LEN-1]);
        while(ans_l > 1) {
            ans[BIGINT_NUMBER_LEN-ans_l] = 0;
            --ans_l;
        }
        ans[BIGINT_NUMBER_LEN-1] = final;
        return final == 0 ? 0:1;
    } else {
        ll modTmpMag[BIGINT_NUMBER_LEN] {};
        bigint_number_divide_and_remain(ans, ans_l, mod, mod_l, NULL, modTmpMag);
        for(int i = 0; i < BIGINT_NUMBER_LEN; ++i)
            ans[i] = modTmpMag[i];

        ans_l = mod_l;
        while(ans[BIGINT_NUMBER_LEN-ans_l] == 0)
            --ans_l;
        return ans_l;
    }
}

BigInt BigInt::pow(BigInt &num, BigInt &mod) {
    if(mod.mag[0] == BIGINT_NUMBER_MAX || mod.bitLen == 0)
        throw range_error("Invalid mod number.");
    if(num.mag[0] == BIGINT_NUMBER_MAX)
        return BigInt(0ll);
    if(isZero(num))
        return BigInt(1ll);
    if(isOne(num))
        return this->mod(mod);

    ll n[BIGINT_NUMBER_LEN], m[BIGINT_NUMBER_LEN], ans[BIGINT_NUMBER_LEN] {};
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        m[i] = num.mag[i];
        n[i] = (mag[0] == BIGINT_NUMBER_MAX)?BIGINT_NUMBER_MAX-mag[i]:mag[i];
    }
    int isNeg = (mag[0] == BIGINT_NUMBER_MAX)&&(m[BIGINT_NUMBER_LEN-1]&1);
    bigint_number_binary_pow(n, bitLen, m, num.bitLen , mod.mag, mod.bitLen, ans);
    if(isNeg) {
        for(ll &i : ans)
            i = BIGINT_NUMBER_MAX - i;
    }
    return BigInt(ans, BIGINT_NUMBER_LEN, !isNeg);
}

int bigint_number_binary_pow(ll *n , int n_l, int m, ll *ans) {
    if(m == 0) {
        ans[BIGINT_NUMBER_LEN-1] = 1;
        return 1;
    }
    if(m == 1) {
        for(int i = BIGINT_NUMBER_LEN - n_l; i < BIGINT_NUMBER_LEN; ++i)
            ans[i] = n[i];
        return n_l;
    }
    int isOdd = (m&1);
    m = (m>>1);
    int ans_l = bigint_number_binary_pow(n, n_l, m, ans);
    ans_l = bigint_number_quick_square(ans, ans_l);

    if(isOdd) {
        bigint_number_quick_mul(ans, ans_l, n, n_l);

        int index = BIGINT_NUMBER_LEN - ans_l - n_l - 1;
        while(ans[index] == 0)
            ++index;
        ans_l = BIGINT_NUMBER_LEN - index;
    }
    return ans_l;
}

BigInt BigInt::pow(int num) {
    if(num+BIGINT_NUMBER_OFFSET*bitLen > BIGINT_NUMBER_MAX)
        throw range_error("Number out of range.");
    if(num < 0)
        return BigInt(0ll);
    if(num == 0)
        return BigInt(1ll);
    if(num == 1)
        return BigInt(mag, BIGINT_NUMBER_LEN, mag[0] == 0);
    ll n[BIGINT_NUMBER_LEN], ans[BIGINT_NUMBER_LEN] {};
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        n[i] = (mag[0] == BIGINT_NUMBER_MAX)?BIGINT_NUMBER_MAX-mag[i]:mag[i];
    }
    int isNeg = (mag[0] == BIGINT_NUMBER_MAX)&&(num&1);
    bigint_number_binary_pow(n, bitLen, num, ans);
    if(isNeg) {
        for(ll &i : ans)
            i = BIGINT_NUMBER_MAX - i;
    }
    return BigInt(ans, BIGINT_NUMBER_LEN, !isNeg);
}

BigInt BigInt::copy() {
    return BigInt(mag, BIGINT_NUMBER_LEN, mag[0] == 0);
}

bool BigInt::equal(BigInt &num) {
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        if(mag[i] != num.mag[i])
            return false;
    }
    return true;
}

int BigInt::compareTo(BigInt &num) {
    bool n_isNeg = (num.mag[0] == BIGINT_NUMBER_MAX), m_isNeg = (mag[0] == BIGINT_NUMBER_MAX);
    if(m_isNeg && !n_isNeg)
        return -1;
    if(n_isNeg && !m_isNeg)
        return 1;
    if(bitLen < num.bitLen)
        return m_isNeg?1:-1;
    if(bitLen > num.bitLen)
        return m_isNeg?-1:1;
    for(int i = BIGINT_NUMBER_LEN-bitLen; i < BIGINT_NUMBER_LEN; ++i) {
        if(num.mag[i] < mag[i]) {
            return 1;
        } else if (num.mag[i] > mag[i]) {
            return -1;
        }
    }
    return 0;
}


void BigInt::parseToMag(string val, int radix) {
    for(ll & i : mag) {
        i = 0;
    }
    if(val.length() == 0)
        return ;

    int cursor = 0, isNeg = 0;
    if(val[0] == '-') {
        cursor = 1;
        isNeg = 1;
    } else if(val[0] == '+') {
        cursor = 1;
    }
    int l = val.length();
    int len = l - cursor;

    int radLen = lenPerRadix[radix];
    int firstLen = len%radLen;
    if (firstLen == 0)
        firstLen = radLen;
    string group = val.substr(cursor, firstLen);
    cursor += firstLen;
    mag[BIGINT_NUMBER_LEN-1] = bigint_number_parse_to_long(group, radix);
    ll superRadix = intRadix[radix], groupVal;
    ll y = superRadix&BIGINT_NUMBER_MAX, z, lastZero = 127;
    while(cursor < l) {
        group = val.substr(cursor, radLen);
        groupVal = bigint_number_parse_to_long(group, radix);
        cursor += radLen;

        z = groupVal&BIGINT_NUMBER_MAX;
        ll product, carry = 0;
        for(int i = BIGINT_NUMBER_LEN-1; i >= 0; --i ) {
            product = y*(mag[i]&BIGINT_NUMBER_MAX)+carry;
            if(product == 0 && i < lastZero)
                break;
            mag[i] = product&BIGINT_NUMBER_MAX;
            carry = product>>BIGINT_NUMBER_OFFSET;
            lastZero = i;
        }

        ll sum = (mag[BIGINT_NUMBER_LEN-1]&BIGINT_NUMBER_MAX) + z;
        mag[BIGINT_NUMBER_LEN-1] = sum&BIGINT_NUMBER_MAX;
        carry = sum >> BIGINT_NUMBER_OFFSET;
        for(int i = BIGINT_NUMBER_LEN-2; i >= 0; --i ) {
            sum = (mag[i]&BIGINT_NUMBER_MAX)+carry;
            if(sum == 0 && i < lastZero)
                break;
            mag[i] = sum&BIGINT_NUMBER_MAX;
            carry = sum>>BIGINT_NUMBER_OFFSET;
            lastZero = i;
        }
    }
    bitLen = 0;
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        if(mag[i] != 0 && bitLen == 0)
            bitLen = BIGINT_NUMBER_LEN -i;
        if(isNeg)
            mag[i] = BIGINT_NUMBER_MAX - mag[i];
    }
}

BigInt BigInt::operator+(BigInt num) {
    return add(num);
}

BigInt BigInt::operator-(BigInt num) {
    return sub(num);
}

BigInt BigInt::operator*(BigInt num) {
    return mul(num);
}

BigInt BigInt::operator/(BigInt num) {
    return div(num);
}

BigInt BigInt::operator%(BigInt num) {
    return remain(num);
}

BigInt BigInt::operator+(const ll num) {
    BigInt n(num);
    return add(n);
}

BigInt BigInt::operator-(const ll num) {
    BigInt n(num);
    return sub(n);
}

BigInt BigInt::operator*(const ll num) {
    BigInt n(num);
    return mul(n);
}

BigInt BigInt::operator/(const ll num) {
    BigInt n(num);
    return div(n);
}

BigInt BigInt::operator%(const ll num) {
    BigInt n(num);
    return remain(n);
}

int bigint_number_radix_div_and_mod(ll *mag, int radix, int *min) {
    int add = 0;
    unsigned ll sum;
    for(int i = *min; i < BIGINT_NUMBER_LEN; ++i) {
        sum = add * BIGINT_NUMBER_RADIX + mag[i];
        mag[i] = sum/radix;
        add = sum%radix;
    }
    while(mag[*min] == 0)
        ++(*min);
    return add;
}

string BigInt::toString() {
    return toString(10);
}

string BigInt::toString(int radix) {
    if(radix < 2 || radix > 16)
        throw runtime_error("Invalid radix.");
    ll new_mag[BIGINT_NUMBER_LEN];
    int isNeg = mag[0] == BIGINT_NUMBER_MAX;
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        if(isNeg)
            new_mag[i] = BIGINT_NUMBER_MAX - mag[i];
        else
            new_mag[i] = mag[i];
    }

    string ans;
    int mod, min = BIGINT_NUMBER_LEN-bitLen;
    while(min < BIGINT_NUMBER_LEN) {
        mod = bigint_number_radix_div_and_mod(new_mag, radix, &min);
        ans = intToHex[mod] + ans;
    }
    if(new_mag[BIGINT_NUMBER_LEN-1] > 0) {
        ans = intToHex[new_mag[BIGINT_NUMBER_LEN-1]] + ans;
    }
    if(ans.length() == 0)
        ans = "0";
    if(isNeg)
        ans = '-' + ans;
    return ans;
}

const char * BigInt::toBytes() {
    ll new_mag[BIGINT_NUMBER_LEN];
    int isNeg = mag[0] == BIGINT_NUMBER_MAX;
    for(int i = 0; i < BIGINT_NUMBER_LEN; ++i) {
        if(isNeg)
            new_mag[i] = BIGINT_NUMBER_MAX - mag[i];
        else
            new_mag[i] = mag[i];
    }
    const int len = bitLen<<2, radix = BIGINT_NUMBER_LEN<<2;
    char tmp[len];
    int mod, index = len - 1, min = BIGINT_NUMBER_LEN-bitLen;
    string out;
    while(min < BIGINT_NUMBER_LEN) {
        mod = bigint_number_radix_div_and_mod(new_mag, radix, &min);
        out = (char)((mod>127)?(mod-256):mod) + out;
    }
    return out.c_str();
}

BigInt::BigInt() {
    for (ll &itm : mag)
        itm = 0;
    bitLen = 0;
}

BigInt::BigInt(const char *str) {
    parseToMag(str, 10);
}

BigInt::BigInt(const char *str, int radix) {
    if (radix < 2 || radix > 16)
        throw range_error("Invalid radix.");
    parseToMag(str, radix);
}

BigInt::BigInt(const ll num) {
    if (num > 0xffffffffffffffff)
        throw runtime_error("input number out of range.");
    ll f = 0, n = num;
    if (num < 0) {
        n = -num;
        f = BIGINT_NUMBER_MAX;
    }
    for (ll &i : mag)
        i = f;
    if (num == 0) {
        bitLen = 0;
        return;
    }
    mag[BIGINT_NUMBER_LEN - 1] = num < 0 ? BIGINT_NUMBER_MAX - (n & BIGINT_NUMBER_MAX) : n & BIGINT_NUMBER_MAX;
    bitLen = 1;
    if ((n >> 16) > 0) {
        mag[BIGINT_NUMBER_LEN - 2] = num < 0 ? BIGINT_NUMBER_MAX - (n >> BIGINT_NUMBER_OFFSET) : (n >> BIGINT_NUMBER_OFFSET);
        bitLen = 2;
    }
}
BigInt::BigInt(const ll *mag_ptr, int len, bool mag_sign) {
    if (len > BIGINT_NUMBER_LEN)
        throw runtime_error("Input mag length too long.");
    ll f = mag_sign ? 0 : BIGINT_NUMBER_MAX;
    bitLen = 0;
    for (int i = BIGINT_NUMBER_LEN - len; i < BIGINT_NUMBER_LEN; ++i) {
        mag[i] = mag_ptr[i];
        if (mag[i] != f && bitLen == 0)
            bitLen = BIGINT_NUMBER_LEN - i;
    }
    for (int i = 0; i < BIGINT_NUMBER_LEN - len; ++i)
        mag[i] = f;
}