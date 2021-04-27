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

@Author archer 2021/3/12.
*/

#include "EcdsaBigInt.h"

namespace EcdsaBigInt {
    BigInt SECP256K1_P("115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
    BigInt SECP256K1_N("115792089237316195423570985008687907852837564279074904382605163141518161494337",10);
    BigInt SECP256K1_A("0",10);
    BigInt SECP256K1_B("7",10);
    BigInt SECP256K1_Gx("55066263022277343669578718895168534326250603453777594175500187360389116729240",10);
    BigInt SECP256K1_Gy("32670510020758816978083085130507043184471273380659243275938904335757337482424",10);

    BigInt inv(BigInt &a, BigInt &b) {
        if (BigInt::isZero(a))
            return ZERO;
        BigInt lm = ONE, hm = ZERO, _1 = ONE;
        BigInt low = a.mod(b), high = b;

        while (low.compareTo(_1) > 0) {
            BigInt r = high.div(low);
            BigInt lmr = lm.mul(r);
            BigInt nm = hm.sub(lmr), ne = high.mod(low);
            hm = lm;
            high = low;
            lm = nm;
            low = ne;
        }
        return lm.mod(b);
    }

    void fastDouble(BigInt &a0, BigInt &a1, BigInt &a2,
                    BigInt *nx, BigInt *ny, BigInt *nz) {

        BigInt ysq = a1.pow(2).mod(SECP256K1_P);
        BigInt _4 = FOUR, _3 = THREE;
        BigInt s = ysq.mul(a0).mul(_4).mod(SECP256K1_P);

        BigInt tmp = a2.pow(4).mul(SECP256K1_A);
        BigInt m = a0.pow(2).mul(_3).add(tmp).mod(SECP256K1_P);

        BigInt _2 = TWO, _8 = EIGHT;
        BigInt double_s = s.mul(_2);
        *nx = m.pow(2).sub(double_s).mod(SECP256K1_P);
        BigInt s_nx = s.sub(*nx);

        BigInt double_ysq_mul_8 = ysq.pow(2).mul(_8);
        BigInt m_s_nx = m.mul(s_nx);

        *ny = m_s_nx.sub(double_ysq_mul_8).mod(SECP256K1_P);
        *nz = a1.mul(a2).mul(_2).mod(SECP256K1_P);
    }

    void fastAdd(
            BigInt &p0, BigInt &p1, BigInt &p2,
            BigInt &q0, BigInt &q1, BigInt &q2,
            BigInt *nx, BigInt *ny, BigInt *nz
    ) {
        BigInt u1 = q2.pow(2).mul(p0).mod(SECP256K1_P);
        BigInt u2 = p2.pow(2).mul(q0).mod(SECP256K1_P);
        BigInt s1 = q2.pow(3).mul(p1).mod(SECP256K1_P);
        BigInt s2 = p2.pow(3).mul(q1).mod(SECP256K1_P);
        if (u1.equal(u2)) {
            if (s1.equal(s2)) {
                fastDouble(p0, p1, p2, nx, ny, nz);
                return;
            }
            *nx = ZERO;
            *ny = ZERO;
            *nz = ONE;
            return;
        }
        BigInt h = u2.sub(u1), r = s2.sub(s1);
        BigInt h2 = h.pow(2).mod(SECP256K1_P);
        BigInt h3 = h2.mul(h).mod(SECP256K1_P);
        BigInt u1h2 = u1.mul(h2).mod(SECP256K1_P);

        BigInt _2 = TWO;
        BigInt double_u1h2 = u1h2.mul(_2);
        *nx = r.pow(2).sub(h3).sub(double_u1h2).mod(SECP256K1_P);

        BigInt u1h2_nx = u1h2.sub(*nx), s1h3 = s1.mul(h3);
        *ny = r.mul(u1h2_nx).sub(s1h3).mod(SECP256K1_P);
        *nz = h.mul(p2).mul(q2).mod(SECP256K1_P);
    }

    void fastMultiply(BigInt &a0, BigInt &a1, BigInt &a2, BigInt &n,
                      BigInt *nx, BigInt *ny, BigInt *nz) {
        if (BigInt::isZero(a1) || BigInt::isZero(n)) {
            *nx = ZERO;
            *ny = ZERO;
            *nz = ONE;
            return;
        }
        if (BigInt::isOne(n)) {
            *nx = a0;
            *ny = a1;
            *nz = a2;
            return;
        }
        if (n.isNeg() || n.compareTo(SECP256K1_N) >= 0) {
            n = n.mod(SECP256K1_N);
        }
        int isOdd = n.isOdd();
        n.half();
        fastMultiply(a0, a1, a2, n, nx, ny, nz);
        BigInt t0 = *nx, t1 = *ny, t2 = *nz;
        fastDouble(t0, t1, t2, nx, ny, nz);
        if (isOdd) {
            BigInt r0 = *nx, r1 = *ny, r2 = *nz;
            fastAdd(r0, r1, r2, a0, a1, a2, nx, ny, nz);
        }
    }

/*
void fastMultiply_copy(BigInt &a0, BigInt &a1, BigInt &a2, BigInt &k,
                   BigInt *nx, BigInt *ny, BigInt *nz) {

    if(BigInt::isZero(a1) || BigInt::isZero(n)) {
        *nx = ZERO;
        *ny = ZERO;
        *nz = ONE;
        return ;
    }
    *nx = a0;
    *ny = a1;
    *nz = a2;
    if(BigInt::isOne(n)) {
        return ;
    }
    if(n.isNeg() || n.compareTo(SECP256K1_N) >= 0) {
        n = n.mod(SECP256K1_N);
    }
    int isOdds[LEN*OFFSET], index = 0;
    while(!BigInt::isOne(n)) {
        isOdds[index++] = n.isOdd();
        n.half();
    }
    for(int i = index-1; i >= 0; --i) {
        BigInt t0 = *nx, t1 = *ny, t2 = *nz;
        fastDouble(t0, t1, t2,  nx, ny, nz);
        if(isOdds[i]) {
            BigInt r0 = *nx, r1 = *ny, r2 = *nz;
            fastAdd(r0, r1, r2 ,a0,a1,a2, nx, ny, nz);
        }
    }
}
*/
    const char *getVStr(long v) {
        return v == 0 ? "00" : "01";
    }

    const char *
    concatBytes(const char *a, int al, const char *b, int bl, const char *c, int cl, const char *d, int dl) {
        char *out = static_cast<char *>(malloc(sizeof(char) * (al + bl + cl + dl)));
        for (int i = 0; i < al; ++i)
            out[i] = a[i];
        for (int i = al; i < al + bl; ++i)
            out[i] = b[i - al];
        for (int i = al + bl; i < al + bl + cl; ++i)
            out[i] = c[i - al - bl];
        for (int i = al + bl + cl; i < al + bl + cl + dl; ++i)
            out[i] = d[i - al - bl - cl];
        return out;
    }

    string sign(string &privateKeyHex, string &hashHex) {
        char v0[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        char k0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        char char0 = 0, char1 = 1;
        char privateKey[32];
        BigInt::hexStrToBytes(privateKeyHex, privateKey);
        char hash[32];
        BigInt::hexStrToBytes(hashHex, hash);
        const char *k1 = Hmac::sha256(k0, 32, concatBytes(v0, 32, &char0, 1, privateKey, 32, hash, 32), 97);
        const char *v1 = Hmac::sha256(k1, 32, v0, 32);
        const char *k2 = Hmac::sha256(k1, 32, concatBytes(v1, 32, &char1, 1, privateKey, 32, hash, 32), 97);
        const char *v2 = Hmac::sha256(k2, 32, v1, 32);
        const char *kv = Hmac::sha256(k2, 32, v2, 32);

        string kvHex;
        BigInt::bytesToHexStr(kv, 32, kvHex);
        BigInt k(kvHex.c_str(), 16), priv(privateKeyHex.c_str(), 16), hashNum(hashHex.c_str(), 16);

        BigInt _1 = ONE, _2 = TWO;

        BigInt pa, pb, pc, kTmp = k;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, kTmp, &pa, &pb, &pc);
        BigInt z = inv(pc, SECP256K1_P);
        BigInt r = z.pow(2).mul(pa).mod(SECP256K1_P), y = z.pow(3).mul(pb).mod(SECP256K1_P);

        BigInt rp = r.mul(priv).add(hashNum);
        BigInt sRaw = inv(k, SECP256K1_N).mul(rp).mod(SECP256K1_N);
        BigInt s = sRaw.mul(_2).compareTo(SECP256K1_N) < 0 ? sRaw : SECP256K1_N.sub(sRaw);

        int v = (y.isOdd()) ^1;
        if (sRaw.mul(_2).compareTo(SECP256K1_N) < 0) {
            v = 0;
        }
        string rStr = r.toString(16), sStr = s.toString(16), vStr = getVStr(v);
        while (rStr.length() < 64)
            rStr = "0" + rStr;
        while (sStr.length() < 64)
            sStr = "0" + sStr;
        return rStr + sStr + vStr;
    }


/**
 * @param hexPubKey public key content bytes.
 * @param hexHash hash content bytes.
 * @param sig signature content.
 *
 * @return if the hash content has been falsified, return false.
 * */
    bool verify(string &pubKeyHex, string &hashHex, string &sig) {
        int sigLen = sig.length();
        if (sigLen != 130)
            throw runtime_error("Invalid signature, " + sig);

        BigInt _0 = ZERO, _1 = ONE;
        BigInt r = BigInt(sig.substr(0, 64).c_str(), 16), s = BigInt(sig.substr(64, 64).c_str(), 16);
        BigInt rmodn = r.mod(SECP256K1_N), smodn = s.mod(SECP256K1_N);
        if (BigInt::isZero(rmodn) || BigInt::isZero(smodn))
            return false;

        BigInt x(pubKeyHex.substr(0, 64).c_str(), 16), y(pubKeyHex.substr(64, 64).c_str(), 16);

        BigInt w = inv(s, SECP256K1_N);
        BigInt z(hashHex.c_str(), 16);

        BigInt u1 = z.mul(w).mod(SECP256K1_N), u2 = r.mul(w).mod(SECP256K1_N);

        BigInt pa, pb, pc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, u1, &pa, &pb, &pc);

        BigInt pz = inv(pc, SECP256K1_P);
        BigInt px = pz.pow(2).mul(pa).mod(SECP256K1_P), py = pz.pow(3).mul(pb).mod(SECP256K1_P);

        fastMultiply(x, y, _1, u2, &pa, &pb, &pc);
        BigInt qz = inv(pc, SECP256K1_P);
        BigInt qx = qz.pow(2).mul(pa).mod(SECP256K1_P), qy = qz.pow(3).mul(pb).mod(SECP256K1_P);

        fastAdd(px, py, _1, qx, qy, _1, &pa, &pb, &pc);
        BigInt gz = inv(pc, SECP256K1_P);
        BigInt gx = gz.pow(2).mul(pa).mod(SECP256K1_P);

        return r.equal(gx);
    }


/**
 * @param privKeyBytes private key content bytes.
 *
 * @return bytes calculate public key bytes from private key bytes.
 * */
    string privateKeyToPublicKey(string &privateKeyHex) {
        BigInt priv(privateKeyHex.c_str(), 16);
        if (priv.compareTo(SECP256K1_N) > 0)
            throw runtime_error("Invalid private key.");
        BigInt _0 = ZERO, _1 = ONE;

        BigInt pa, pb, pc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, priv, &pa, &pb, &pc);

        BigInt z = inv(pc, SECP256K1_P);
        BigInt x = z.pow(2).mul(pa).mod(SECP256K1_P), y = z.pow(3).mul(pb).mod(SECP256K1_P);
        string xStr = x.toString(16), yStr = y.toString(16);
        while (xStr.length() < 64)
            xStr = "0" + xStr;
        while (yStr.length() < 64)
            yStr = "0" + yStr;
        return xStr + yStr;
    }

/**
 * @param hexHash hash content bytes.
 * @param sig signature content.
 *
 * @return bytes calculate public key bytes from signature and hash.
 * */
    string recoverToPublicKey(const string &sig, string &hashHex) {
        int sigLen = sig.length();
        if (sigLen != 130)
            throw runtime_error("Invalid signature, " + sig);
        BigInt _27 = BigInt("27", 10), _34 = BigInt("34", 10);
        BigInt r(sig.substr(0, 64).c_str(), 16), s(sig.substr(64, 64).c_str(), 16);

        BigInt rmodN = r.mod(SECP256K1_N), smodN = s.mod(SECP256K1_N);
        if (BigInt::isZero(rmodN) || BigInt::isZero(smodN))
            throw runtime_error("Invalid signature, " + sig);

        BigInt v(sig.substr(128, 2).c_str(), 16);
        v = v.add(_27);
        if (v.compareTo(_27) < 0 || v.compareTo(_34) > 0)
            throw runtime_error("Invalid signature, " + sig);

        BigInt x = r;
        BigInt xAB = x.mul(SECP256K1_A).add(SECP256K1_B);
        BigInt num = x.pow(3).add(xAB).mod(SECP256K1_P);

        BigInt _1 = ONE, _2 = TWO, _4 = FOUR;
        BigInt powP = SECP256K1_P.add(_1).div(_4);
        BigInt y = num.pow(powP, SECP256K1_P);
        long yMod2 = y.isOdd(), vMod2 = v.isOdd();
        if (yMod2 ^ vMod2 == 0)
            y = SECP256K1_P.sub(y);

        BigInt ynp = y.pow(2).sub(num).mod(SECP256K1_P);
        if (!BigInt::isZero(ynp))
            throw runtime_error("Invalid signature, " + sig);

        BigInt z(hashHex.c_str(), 16);
        BigInt NzN = SECP256K1_N.sub(z).mod(SECP256K1_N);
        BigInt ga, gb, gc;
        BigInt xya, xyb, xyc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, NzN, &ga, &gb, &gc);
        fastMultiply(x, y, _1, s, &xya, &xyb, &xyc);

        BigInt qzx, qzy, qzz;
        fastAdd(ga, gb, gc, xya, xyb, xyc, &qzx, &qzy, &qzz);
        BigInt QRZ = inv(r, SECP256K1_N);
        BigInt qx, qy, qz;
        fastMultiply(qzx, qzy, qzz, QRZ, &qx, &qy, &qz);

        BigInt pubZ = inv(qz, SECP256K1_P);
        BigInt left = pubZ.pow(2).mul(qx).mod(SECP256K1_P), right = pubZ.pow(3).mul(qy).mod(SECP256K1_P);

        string leftStr = left.toString(16), rightStr = right.toString(16);
        while (leftStr.length() < 64)
            leftStr = "0" + leftStr;
        while (rightStr.length() < 64)
            rightStr = "0" + rightStr;
        return leftStr + rightStr;
    }

}