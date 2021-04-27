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

#include "Int.h"

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

bool int_number_check_char(char c) {
    return (48 <= c && c <= 57)||(65 <= c && c <= 70)||(97 <= c && c <= 102);
}

static ll int_number_parse_to_long(string &s, int radix) {
    int i = (int)s.length()-1;
    ll r = 1, sum = 0;
    while(i >= 0) {
        if(!int_number_check_char(s[i]))
            throw runtime_error("Invalid number, "+s);

        sum += r*hexToInt[s[i]];
        r *= radix;
        --i;
    }
    return sum;
}
int Int::isZero(Int &n) {
    return n.m_bitLen == 0;
}

int Int::isOne(Int &n) {
    return (n.m_bitLen==1)&&(n.m_mag[INT_NUMBER_LEN-1] == 1);
}

void Int::bytesToHexStr(const char *bytes, const int bytesLen, string &outHexStr) {
    outHexStr = "";
    for(int i = 0; i < bytesLen; ++i) {
        int k = bytes[i];
        if(k < 0)
            k += 256;
        outHexStr += intToHex[k>>4];
        outHexStr += intToHex[k&0xf];
    }
}

void Int::hexStrToBytes(string &hex, char *outBytes) {
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

int Int::isOdd() {
    return (m_mag[INT_NUMBER_LEN-1]&1)==1;
}

int Int::isNeg() {
    return !m_isPos;
}

int int_number_compare(ll *n, int n_l, ll *m, int m_l) {
    if(n_l == m_l) {
        for(int i = INT_NUMBER_LEN - n_l; i < INT_NUMBER_LEN; ++i) {
            if(n[i] > m[i])
                return 1;
            if(n[i] < m[i])
                return -1;
        }
        return 0;
    }
    return n_l>m_l?1:-1;
}

int int_number_add(const ll *n, int n_l , const ll *m, int m_l, ll *out) {
    int l = m_l>n_l?m_l:n_l;
    int add = 0;
    ll m_v, n_v;
    for(int i = INT_NUMBER_LEN - 1; i >= INT_NUMBER_LEN - l - 1; --i) {
        m_v = 0;
        if(i >= INT_NUMBER_LEN - m_l)
            m_v = m[i];
        n_v = 0;
        if(i >= INT_NUMBER_LEN - n_l)
            n_v = n[i];

        if(m_v + n_v + add >= INT_NUMBER_RADIX) {
            out[i] = m_v + n_v + add - INT_NUMBER_RADIX;
            add = 1;
        } else {
            out[i] = m_v + n_v + add;
            add = 0;
        }
    }
    if(out[INT_NUMBER_LEN - l - 1] == 0)
        return l;
    return l+1;
}

int int_number_sub(const ll *n, int n_l, const ll *m, int m_l, ll *out) {
    int bow = 0;
    ll m_v, n_v;
    for(int i = INT_NUMBER_LEN - 1; i >= INT_NUMBER_LEN - n_l; --i) {
        m_v = 0;
        n_v = n[i];
        if(i >= INT_NUMBER_LEN - m_l)
            m_v = m[i];

        if(n_v - bow - m_v < 0) {
            out[i] = n_v - m_v - bow + INT_NUMBER_RADIX;
            bow = 1;
        } else {
            out[i] = n_v - m_v - bow;
            bow = 0;
        }
    }
    while(out[INT_NUMBER_LEN - n_l] == 0)
        --n_l;
    return n_l;
}


Int Int::add(Int &num) {
    ll out[INT_NUMBER_LEN]{};
    int bitLen = 0;
    if((m_isPos && num.m_isPos) || (!m_isPos && !num.m_isPos)) {
        bitLen = int_number_add(m_mag, m_bitLen, num.m_mag, num.m_bitLen, out);
        return Int(out, bitLen, m_isPos);
    } else {
        int compare_v = int_number_compare(m_mag, m_bitLen, num.m_mag, num.m_bitLen);
        if(compare_v > 0) {
            bitLen = int_number_sub(m_mag, m_bitLen, num.m_mag, num.m_bitLen, out);
            return Int(out, bitLen, m_isPos);
        } else if(compare_v < 0) {
            bitLen = int_number_sub(num.m_mag, num.m_bitLen, m_mag, m_bitLen, out);
            return Int(out, bitLen, num.m_isPos);
        } else
            return Int(0ll);
    }
}

Int Int::sub(Int &num) {
    Int num_copy(num.m_mag, num.m_bitLen, !num.m_isPos);
    return add(num_copy);
}

int int_number_quick_mul(ll* n, int n_l, ll* m, int m_l, ll* out) {
    int index;
    unsigned ll mul, add = 0;
    for(int i = 0; i <= n_l; ++i) {
        for(int j = 0; j <= m_l; ++j) {
            index = INT_NUMBER_LEN - i - j - 1;
            mul = n[INT_NUMBER_LEN-i-1]*m[INT_NUMBER_LEN-j-1];
            out[index] = (mul&INT_NUMBER_MAX) + out[index] + add;
            add = (mul>>INT_NUMBER_OFFSET) + (out[index]>>INT_NUMBER_OFFSET);
            out[index] = out[index]&INT_NUMBER_MAX;
        }
    }
    index = INT_NUMBER_LEN - m_l - n_l - 1;
    while(out[index] == 0)
        ++index;
    return INT_NUMBER_LEN - index;
}

Int Int::mul(Int &num) {
    if(isZero(num))
        return Int(0ll);
    if (isOne(num))
        return Int(m_mag, m_bitLen, m_isPos);

    ll n[INT_NUMBER_LEN]{};
    int bitLen = int_number_quick_mul(m_mag, m_bitLen, num.m_mag, num.m_bitLen, n);
    bool isPos = (m_isPos+num.m_isPos)!=1;
    return Int(n, bitLen, isPos);
}

ll int_number_predict_div(ll high_n, ll low_n, ll d) {
    unsigned ll sum = high_n*INT_NUMBER_RADIX+low_n;
    return sum/d;
}

/**
 * @deprecated
 * */
ll int_number_get_remain(ll *n, int n_s, int n_e, ll *m, int m_s, int m_e, ll s, ll e) {
    ll mul = (s+e)>>1;
    if(s >= e - 1) {
        int l = n_e - n_s, offset = (n_e - n_s == m_e - m_s)?0:-1, add = 0;
        for(int i = l; i >= 0; --i) {
            if(n[i+n_s] + add < m[i+m_s+offset]) {
                n[i+n_s] = INT_NUMBER_RADIX + n[i+n_s] + add - m[i+m_s+offset];
                add = -1;
            } else {
                n[i+n_s] = n[i+n_s] + add - m[i+m_s+offset];
                add = 0;
            }
        }
        return mul;
    }
    unsigned ll mv = mul*m[m_s], nv;
    if(n_e - n_s == m_e - m_s) {
        nv = n[n_s];
    } else {
        nv = ((unsigned ll )n[n_s]*INT_NUMBER_RADIX) + n[n_s+1];
    }
    if(mv > nv) {
        return int_number_get_remain(n, n_s, n_e, m, m_s, m_e, s, mul);
    }
    if(mv + m[m_s] < nv - mul) {
        return int_number_get_remain(n, n_s, n_e, m, m_s, m_e, mul, e);
    }

    ll mul_ans[INT_NUMBER_LEN] {}, tmp_remain[INT_NUMBER_LEN]{}, borrow = 0;
    unsigned ll sum, add = 0;
    for(int i = m_e; i >= m_s -1; --i) {
        sum = m[i]*mul + add;
        add = (sum>>INT_NUMBER_OFFSET);
        mul_ans[i] = sum&INT_NUMBER_MAX;

        //m_e - i
        if(n[n_e-m_e+i] + borrow < mul_ans[i]) {
            tmp_remain[n_e-m_e+i] = INT_NUMBER_RADIX + n[n_e-m_e+i] + borrow - mul_ans[i];
            borrow = -1;
        } else {
            tmp_remain[n_e-m_e+i] = n[n_e-m_e+i] + borrow - mul_ans[i];
            borrow = 0;
        }
    }
    if(borrow < 0) {
        return int_number_get_remain(n, n_s, n_e, m, m_s, m_e, s, mul);
    }
    borrow = 0;
    int l = n_e - n_s, offset = (n_e - n_s == m_e - m_s)?0:-1;
    for(int i = 0; i<= l; ++i) {
        if(tmp_remain[i+n_s] < m[i+m_s+offset]) {
            borrow = -1;
            break;
        }
        if(tmp_remain[i+n_s] > m[i+m_s+offset]) {
            borrow = 1;
            break;
        }
    }
    if(borrow == 0) {
        for(int i = n_s; i <= n_e; ++i)
            n[i] = 0;
        return mul;
    }
    if(borrow > 0) {
        return int_number_get_remain(n, n_s, n_e, m, m_s, m_e, mul, e);
    }
    memcpy(n+n_s, tmp_remain+n_s, sizeof(ll)*(n_e-n_s+1));
    return mul;
}

/**
 * @deprecated
 * */
int int_number_divide_and_remain_deprecated(const ll *n, int n_l, ll *m, int m_l, ll *div, ll *remain) {
    int n_s = INT_NUMBER_LEN-n_l, n_e = n_s+m_l-1, m_s = INT_NUMBER_LEN-m_l, m_e = INT_NUMBER_LEN-1;
    memcpy(remain, n, sizeof(ll)*INT_NUMBER_LEN);
    while(n_e < INT_NUMBER_LEN) {
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
        if(n_e >= INT_NUMBER_LEN)
            break;

        ll predict_low, predict_high;
        if(n_e-n_s == m_e-m_s) {
            predict_high = remain[n_s]/m[m_s]+1;
            predict_low = 0;
        } else {
            predict_high = int_number_predict_div(remain[n_s], remain[n_s+1], m[m_s]) + 1;
            predict_low = remain[n_s]/m[m_s];
        }


        div[n_e] = int_number_get_remain(remain, n_s, n_e, m, m_s, m_e, predict_low, predict_high);
        while(remain[n_s] == 0)
            ++n_s;
        ++n_e;
    }
    int index = INT_NUMBER_LEN-n_l+m_l-1;
    while(div[index] == 0)
        ++index;
    return INT_NUMBER_LEN - index;
}

int int_number_mul_sub(ll *n, int n_s, int n_e, ll *m, int m_s, int m_e, ll mul, ll *rem) {
    ll mul_ans;
    int borrow = 0;
    unsigned ll sum, add = 0;
    for(int i = m_e; i >= m_s -1; --i) {
        sum = m[i]*mul + add;
        add = (sum>>INT_NUMBER_OFFSET);
        mul_ans = sum&INT_NUMBER_MAX;

        //m_e - i
        if(n[n_e-m_e+i] + borrow < mul_ans) {
            rem[n_e-m_e+i] = n[n_e-m_e+i] + INT_NUMBER_RADIX + borrow - mul_ans;
            borrow = -1;
        } else {
            rem[n_e-m_e+i] = n[n_e-m_e+i] + borrow - mul_ans;
            borrow = 0;
        }
    }
    return borrow;
}

void int_number_divide_and_remain(const ll *n, int n_l, ll *m, int m_l, ll *div, ll *remain) {
    int n_s = INT_NUMBER_LEN-n_l, n_e = n_s+m_l-1, m_s = INT_NUMBER_LEN-m_l, m_e = INT_NUMBER_LEN-1;
    memcpy(remain, n, sizeof(ll)*INT_NUMBER_LEN);
    while(n_e < INT_NUMBER_LEN) {
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
        if(n_e >= INT_NUMBER_LEN)
            break;

        ll predict_div;
        if(n_e-n_s == m_e-m_s) {
            predict_div = remain[n_s]/m[m_s];
        } else {
            predict_div = int_number_predict_div(remain[n_s], remain[n_s+1], m[m_s]);
        }
        ll rem[INT_NUMBER_LEN] {};
        while(int_number_mul_sub(remain, n_s, n_e, m, m_s, m_e,predict_div, rem) < 0)
            --predict_div;
        memcpy(remain+n_s, rem+n_s, sizeof(ll)*(n_e-n_s+1));
        div[n_e] = predict_div;
        while(remain[n_s] == 0)
            ++n_s;
        ++n_e;
    }
}


int int_number_one_bit_div(const ll *n, int n_l, ll mul, ll* div) {
    int min = INT_NUMBER_LEN-n_l;
    unsigned ll sum, mod = 0;
    for(int i = min; i < INT_NUMBER_LEN; ++i) {
        sum = n[i] + mod*INT_NUMBER_RADIX;
        div[i] = sum/mul;
        mod = sum%mul;
    }
    while(div[INT_NUMBER_LEN - n_l] == 0)
        --n_l;
    return n_l;
}

Int Int::div(Int &num) {
    if(isZero(num))
        throw range_error("Divided by zero.");
    if(isOne(num))
        return Int(m_mag, m_bitLen, m_isPos);

    int compare_v = int_number_compare(m_mag, m_bitLen, num.m_mag, num.m_bitLen);
    if(compare_v == 0)
        return Int(1ll);
    if(compare_v < 0)
        return Int(0ll);

    int bitLen;
    ll div[INT_NUMBER_LEN] {};
    if(num.m_bitLen == 1) {
        bitLen = int_number_one_bit_div(m_mag, m_bitLen, num.m_mag[INT_NUMBER_LEN-1], div);
    } else {
        ll remain[INT_NUMBER_LEN];
        int_number_divide_and_remain(m_mag, m_bitLen, num.m_mag, num.m_bitLen, div, remain);
        bitLen = INT_NUMBER_LEN-m_bitLen+num.m_bitLen-1;
        while(div[INT_NUMBER_LEN-bitLen] == 0)
            --bitLen;
    }
    bool isPos = (m_isPos+num.m_isPos)!=1;
    return Int(div, bitLen, isPos);
}

ll int_number_one_bit_mod(const ll *n, int n_l, ll mul) {
    int min = INT_NUMBER_LEN-n_l;
    unsigned ll sum, mod = 0;
    for(int i = min; i < INT_NUMBER_LEN; ++i) {
        sum = n[i] + mod*INT_NUMBER_RADIX;
        mod = sum%mul;
    }
    return mod;
}

Int Int::remain(Int &num) {
    if (isZero(num)) {
        throw range_error("Divided by zero in mod calculate.");
    }
    if (!num.m_isPos) {
        throw range_error("Negative number in mod calculate.");
    }
    if(isOne(num))
        return Int(0ll);

    int compare_v = int_number_compare(m_mag, m_bitLen, num.m_mag, num.m_bitLen);
    if(compare_v == 0)
        return Int(0ll);
    if(compare_v < 0)
        return Int(m_mag, m_bitLen, m_isPos);

    if(num.m_bitLen == 1) {
        ll mod = int_number_one_bit_mod(m_mag, m_bitLen, num.m_mag[INT_NUMBER_LEN-1]);
        return Int(mod*(m_isPos?1:-1));
    } else {
        ll div[INT_NUMBER_LEN]{}, remain[INT_NUMBER_LEN];
        int_number_divide_and_remain(m_mag, m_bitLen, num.m_mag, num.m_bitLen, div, remain);
        int bitLen = num.m_bitLen;
        while(remain[INT_NUMBER_LEN-bitLen] == 0)
            --bitLen;
        return Int(remain, bitLen, m_isPos);
    }
}

Int Int::mod(Int &num) {
    Int rest = remain(num);
    if(!rest.m_isPos) {
        ll out[INT_NUMBER_LEN]{};
        int bitLen = int_number_sub(num.m_mag, num.m_bitLen, rest.m_mag, rest.m_bitLen, out);
        return Int(out, bitLen, true);
    }
    return rest;
}

void Int::half() {
    unsigned ll add = 0, m;
    int min = INT_NUMBER_LEN-m_bitLen;
    for(int i = min; i < INT_NUMBER_LEN; ++i) {
        m = m_mag[i] + add*INT_NUMBER_RADIX;
        add = m&1;
        m_mag[i] = m>>1;
    }
    if(m_mag[INT_NUMBER_LEN-m_bitLen] == 0)
        --m_bitLen;
}

void Int::doubleSelf() {
    unsigned ll m, add = 0;
    int min = (m_bitLen == INT_NUMBER_LEN)?0:INT_NUMBER_LEN-m_bitLen-1;
    for(int i = INT_NUMBER_LEN - 1; i >= min; --i) {
        m = (m_mag[i]<<1) + add;
        m_mag[i] = m&INT_NUMBER_MAX;
        add = m>>INT_NUMBER_OFFSET;
    }
    if(m_mag[INT_NUMBER_LEN-m_bitLen-1] > 0)
        ++m_bitLen;
}

int int_number_quick_square(ll *n, int n_l, ll *out) {
    int index;
    unsigned ll mul, add = 0;
    for(int i = 0; i <= n_l; ++i) {
        for(int j = 0; j <= n_l; ++j) {
            index = INT_NUMBER_LEN - i - j - 1;
            mul = n[INT_NUMBER_LEN-i-1]*n[INT_NUMBER_LEN-j-1];
            out[index] = (mul&INT_NUMBER_MAX) + out[index] + add;
            add = (mul>>INT_NUMBER_OFFSET) + (out[index]>>INT_NUMBER_OFFSET);
            out[index] = out[index]&INT_NUMBER_MAX;
        }
    }
    index = INT_NUMBER_LEN - 2 * n_l - 1;
    while(out[index] == 0)
        ++index;
    return INT_NUMBER_LEN - index;
}

int int_number_binary_pow(ll *n, int n_l, ll *m, int m_l, ll *mod, int mod_l, ll *ans, bool first) {
    if(m_l == 0) {
        ans[INT_NUMBER_LEN-1] = 1;
        return 1;
    }
    if(m_l == 1 && m[INT_NUMBER_LEN-1] == 1) {
        memcpy(ans, n, sizeof(ll)*INT_NUMBER_LEN);
        return n_l;
    }
    /*
     * quick pow here.
     * */
    int isOdd = m[INT_NUMBER_LEN-1]&1;
    unsigned ll add = 0, sum;
    for(int i = INT_NUMBER_LEN-m_l; i < INT_NUMBER_LEN; ++i) {
        sum = m[i] + add*INT_NUMBER_RADIX;
        add = sum&1;
        m[i] = sum>>1;
    }
    if(m[INT_NUMBER_LEN-m_l] == 0)
        --m_l;
    int ans_l = int_number_binary_pow(n, n_l, m, m_l, mod, mod_l, ans, false);
    ll sq_out[INT_NUMBER_LEN]{};
    int sq_out_l = int_number_quick_square(ans, ans_l, sq_out);
    if(isOdd) {
        ll mul_out[INT_NUMBER_LEN]{};
        ans_l = int_number_quick_mul(sq_out, sq_out_l, n, n_l, mul_out);
        memcpy(ans, mul_out, sizeof(ll)*INT_NUMBER_LEN);
    } else {
        memcpy(ans, sq_out, sizeof(ll)*INT_NUMBER_LEN);
        ans_l = sq_out_l;
    }
    /*
     * quick mod here
     * */
    if((2*ans_l + n_l < INT_NUMBER_LEN) && !first)
        return ans_l;
    if(mod_l == 1) {
        ll mod_v = int_number_one_bit_mod(ans, ans_l, mod[INT_NUMBER_LEN-1]);
        while(ans_l > 1) {
            ans[INT_NUMBER_LEN-ans_l] = 0;
            --ans_l;
        }
        ans[INT_NUMBER_LEN-1] = mod_v;
        return mod_v == 0 ? 0:1;
    } else {
        ll div_v[INT_NUMBER_LEN] {}, mod_v[INT_NUMBER_LEN];
        int_number_divide_and_remain(ans, ans_l, mod, mod_l, div_v, mod_v);
        memcpy(ans, mod_v, sizeof(ll)*INT_NUMBER_LEN);
        ans_l = mod_l;
        while(ans[INT_NUMBER_LEN - ans_l] == 0)
            --ans_l;
        return ans_l;
    }
}

Int Int::pow(Int &num, Int &mod) {
    if(mod.m_bitLen == 0 || !mod.m_isPos)
        throw range_error("Invalid mod number.");
    if(!num.m_isPos)
        return Int(0ll);
    if(isZero(num))
        return Int(1ll);
    if(isOne(num))
        return this->mod(mod);

    ll m[INT_NUMBER_LEN], ans[INT_NUMBER_LEN] {};
    memcpy(m, num.m_mag, sizeof(ll)*INT_NUMBER_LEN);
    int ans_l = int_number_binary_pow(m_mag, m_bitLen, m, num.m_bitLen , mod.m_mag, mod.m_bitLen, ans, true);
    return Int(ans, ans_l, true);
}

int int_number_binary_pow(ll *n , int n_l, int m, ll *ans) {
    if(m == 0) {
        ans[INT_NUMBER_LEN-1] = 1;
        return 1;
    }
    if(m == 1) {
        memcpy(ans, n, sizeof(ll)*INT_NUMBER_LEN);
        return n_l;
    }
    int isOdd = (m&1);

    int ans_l = int_number_binary_pow(n, n_l, m>>1, ans);
    ll sq_out[INT_NUMBER_LEN] {};
    int sq_out_l = int_number_quick_square(ans, ans_l, sq_out);
    if(isOdd) {
        ll mul_out[INT_NUMBER_LEN] {};
        ans_l = int_number_quick_mul(sq_out, sq_out_l, n, n_l, mul_out);
        memcpy(ans, mul_out, sizeof(ll)*INT_NUMBER_LEN);
    } else {
        memcpy(ans, sq_out, sizeof(ll)*INT_NUMBER_LEN);
        ans_l = sq_out_l;
    }
    return ans_l;
}

Int Int::pow(int num) {
    if(num+INT_NUMBER_OFFSET*m_bitLen > INT_NUMBER_MAX)
        throw range_error("Number out of range.");
    if(num < 0)
        return Int(0ll);
    ll ans[INT_NUMBER_LEN] {};
    int ans_l = int_number_binary_pow(m_mag, m_bitLen, num, ans);
    bool isPos = (num&1)==0 || m_isPos;
    return Int(ans, ans_l, isPos);
}

Int Int::copy() {
    return Int(m_mag, m_bitLen, m_isPos);
}

bool Int::equal(Int &num) {
    return compareTo(num) == 0;
}

int Int::compareTo(Int &num) {
    if(m_isPos && !num.m_isPos)
        return 1;
    if(!m_isPos && num.m_isPos)
        return -1;
    if(m_bitLen > num.m_bitLen)
        return m_isPos?1:-1;
    if(m_bitLen < num.m_bitLen)
        return m_isPos?-1:1;
    for(int i = INT_NUMBER_LEN-m_bitLen; i < INT_NUMBER_LEN; ++i) {
        if(num.m_mag[i] < m_mag[i]) {
            return 1;
        } else if (num.m_mag[i] > m_mag[i]) {
            return -1;
        }
    }
    return 0;
}


void Int::parseToMag(string val, int radix) {
    for(ll &itm : m_mag)
        itm = 0;

    m_isPos = true;
    m_bitLen = 0;
    if(val.length() == 0) {
        return ;
    }
    int cursor = 0;
    if(val[0] == '-') {
        cursor = 1;
        m_isPos = false;
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
    m_mag[INT_NUMBER_LEN-1] = int_number_parse_to_long(group, radix);
    ll superRadix = intRadix[radix], groupVal;
    ll y = superRadix&INT_NUMBER_MAX, z, lastZero = 127;
    while(cursor < l) {
        group = val.substr(cursor, radLen);
        groupVal = int_number_parse_to_long(group, radix);
        cursor += radLen;

        z = groupVal&INT_NUMBER_MAX;
        ll product, carry = 0;
        for(int i = INT_NUMBER_LEN-1; i >= 0; --i ) {
            product = y*(m_mag[i]&INT_NUMBER_MAX)+carry;
            if(product == 0 && i < lastZero) {
                m_bitLen = INT_NUMBER_LEN - i - 1;
                break;
            }
            m_mag[i] = product&INT_NUMBER_MAX;
            carry = product>>INT_NUMBER_OFFSET;
            lastZero = i;
        }

        ll sum = (m_mag[INT_NUMBER_LEN-1]&INT_NUMBER_MAX) + z;
        m_mag[INT_NUMBER_LEN-1] = sum&INT_NUMBER_MAX;
        carry = sum >> INT_NUMBER_OFFSET;
        for(int i = INT_NUMBER_LEN-2; i >= 0; --i ) {
            sum = (m_mag[i]&INT_NUMBER_MAX)+carry;
            if(sum == 0 && i < lastZero) {
                m_bitLen = INT_NUMBER_LEN - i - 1;
                break;
            }
            m_mag[i] = sum&INT_NUMBER_MAX;
            carry = sum>>INT_NUMBER_OFFSET;
            lastZero = i;
        }
    }
    if(m_bitLen == 0) {
        int index = 0;
        while(m_mag[index] == 0)
            ++index;
        m_bitLen = INT_NUMBER_LEN - index;
    }
}

Int Int::operator+(Int num) {
    return add(num);
}

Int Int::operator-(Int num) {
    return sub(num);
}

Int Int::operator*(Int num) {
    return mul(num);
}

Int Int::operator/(Int num) {
    return div(num);
}

Int Int::operator%(Int num) {
    return remain(num);
}

Int Int::operator+(const ll num) {
    Int n(num);
    return add(n);
}

Int Int::operator-(const ll num) {
    Int n(num);
    return sub(n);
}

Int Int::operator*(const ll num) {
    Int n(num);
    return mul(n);
}

Int Int::operator/(const ll num) {
    Int n(num);
    return div(n);
}

Int Int::operator%(const ll num) {
    Int n(num);
    return remain(n);
}

int int_number_radix_div_and_mod(ll *mag, int radix, int *index) {
    int add = 0;
    unsigned ll sum;
    for(int i = *index; i < INT_NUMBER_LEN; ++i) {
        sum = add * INT_NUMBER_RADIX + mag[i];
        mag[i] = sum/radix;
        add = sum%radix;
    }
    while(mag[*index] == 0)
        ++(*index);
    return add;
}

string Int::toString() {
    return toString(10);
}

string Int::toString(int radix) {
    if(radix < 2 || radix > 16)
        throw runtime_error("Invalid radix.");
    ll remain_mag[INT_NUMBER_LEN];
    memcpy(remain_mag, m_mag, sizeof(ll)*INT_NUMBER_LEN);
    string ans;
    int mod, index = INT_NUMBER_LEN-m_bitLen;
    while(index < INT_NUMBER_LEN) {
        mod = int_number_radix_div_and_mod(remain_mag, radix, &index);
        ans = intToHex[mod] + ans;
    }
    if(remain_mag[INT_NUMBER_LEN-1] > 0) {
        ans = intToHex[remain_mag[INT_NUMBER_LEN-1]] + ans;
    }
    if(ans.length() == 0)
        ans = "0";
    if(!m_isPos)
        ans = '-' + ans;
    return ans;
}

const char * Int::toBytes() {
    ll new_mag[INT_NUMBER_LEN];
    memcpy(new_mag, m_mag, sizeof(ll)*INT_NUMBER_LEN);
    int mod, index = (m_bitLen<<2) - 1, min = INT_NUMBER_LEN-m_bitLen, radix = INT_NUMBER_LEN<<2;
    string out;
    while(min < INT_NUMBER_LEN) {
        mod = int_number_radix_div_and_mod(new_mag, radix, &min);
        out = (char)((mod>127)?(mod-256):mod) + out;
    }
    return out.c_str();
}

Int::Int(const char *str) {
    parseToMag(str, 10);
}

Int::Int(const char *str, int radix) {
    if (radix < 2 || radix > 16)
        throw range_error("Invalid radix.");
    parseToMag(str, radix);
}

Int::Int() {
    for (ll &itm : m_mag)
        itm = 0;
    m_isPos = true;
    m_bitLen = 0;
}

Int::Int(ll num) {
    if (num > 0xffffffffffffffff)
        throw runtime_error("input number out of range.");

    for (ll &i : m_mag)
        i = 0;
    m_isPos = true;
    if (num == 0) {
        m_bitLen = 0;
        return ;
    }

    ll n = num;
    if (num < 0) {
        m_isPos = false;
        n = -num;
    }
    m_mag[INT_NUMBER_LEN - 1] = n & INT_NUMBER_MAX;
    m_bitLen = 1;
    if ((n >> INT_NUMBER_OFFSET) > 0) {
        m_mag[INT_NUMBER_LEN - 2] = n >> INT_NUMBER_OFFSET;
        m_bitLen = 2;
    }
}

Int::Int(const ll *mag, int bitLen, bool isPos) {
    memcpy(m_mag, mag, sizeof(ll)*INT_NUMBER_LEN);
    m_bitLen = bitLen;
    m_isPos = isPos;
}