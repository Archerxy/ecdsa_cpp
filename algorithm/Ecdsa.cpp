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

@Author archer 2021/3/15.
*/


#include "Ecdsa.h"

namespace Ecdsa {
    Int SECP256K1_P("115792089237316195423570985008687907853269984665640564039457584007908834671663", 10);
    Int SECP256K1_N("115792089237316195423570985008687907852837564279074904382605163141518161494337",10);
    Int SECP256K1_A("0",10);
    Int SECP256K1_B("7",10);
    Int SECP256K1_Gx("55066263022277343669578718895168534326250603453777594175500187360389116729240",10);
    Int SECP256K1_Gy("32670510020758816978083085130507043184471273380659243275938904335757337482424",10);

    Int inv(Int &a, Int &b) {
        if (Int::isZero(a))
            return Int(0ll);
        Int lm = Int(1ll), hm = Int(0ll), _1 = Int(1ll);
        Int low = a.mod(b), high = b;

        while (low.compareTo(_1) > 0) {
            Int r = high.div(low);
            Int lmr = lm.mul(r);
            Int nm = hm.sub(lmr), ne = high.mod(low);
            hm = lm;
            high = low;
            lm = nm;
            low = ne;
        }
        return lm.mod(b);
    }

    void fastDouble(Int &a0, Int &a1, Int &a2,
                    Int *nx, Int *ny, Int *nz
                    ) {
        Int ysq = a1.pow(2).mod(SECP256K1_P);
        Int _4 = Int(4ll), _3 = Int(3ll);
        Int s = ysq.mul(a0).mul(_4).mod(SECP256K1_P);
        Int tmp = a2.pow(4).mul(SECP256K1_A);
        Int m = a0.pow(2).mul(_3).add(tmp).mod(SECP256K1_P);

        Int _2 = Int(2ll), _8 = Int(8ll);
        Int double_s = s.mul(_2);
        *nx = m.pow(2).sub(double_s).mod(SECP256K1_P);
        Int s_nx = s.sub(*nx);

        Int double_ysq_mul_8 = ysq.pow(2).mul(_8);
        Int m_s_nx = m.mul(s_nx);

        *ny = m_s_nx.sub(double_ysq_mul_8).mod(SECP256K1_P);
        *nz = a1.mul(a2).mul(_2).mod(SECP256K1_P);
    }

    void fastAdd(
            Int &p0, Int &p1, Int &p2,
            Int &q0, Int &q1, Int &q2,
            Int *nx, Int *ny, Int *nz
            ) {
        Int u1 = q2.pow(2).mul(p0).mod(SECP256K1_P);
        Int u2 = p2.pow(2).mul(q0).mod(SECP256K1_P);
        Int s1 = q2.pow(3).mul(p1).mod(SECP256K1_P);
        Int s2 = p2.pow(3).mul(q1).mod(SECP256K1_P);
        if (u1.equal(u2)) {
            if (s1.equal(s2)) {
                fastDouble(p0, p1, p2, nx, ny, nz);
                return;
            }
            *nx = Int(0ll);
            *ny = Int(0ll);
            *nz = Int(1ll);
            return;
        }
        Int h = u2.sub(u1), r = s2.sub(s1);
        Int h2 = h.pow(2).mod(SECP256K1_P);
        Int h3 = h2.mul(h).mod(SECP256K1_P);
        Int u1h2 = u1.mul(h2).mod(SECP256K1_P);

        Int _2 = Int(2ll);
        Int double_u1h2 = u1h2.mul(_2);
        *nx = r.pow(2).sub(h3).sub(double_u1h2).mod(SECP256K1_P);

        Int u1h2_nx = u1h2.sub(*nx), s1h3 = s1.mul(h3);
        *ny = r.mul(u1h2_nx).sub(s1h3).mod(SECP256K1_P);
        *nz = h.mul(p2).mul(q2).mod(SECP256K1_P);
    }

    void fastMultiply(Int &a0, Int &a1, Int &a2, Int &n,
                      Int *nx, Int *ny, Int *nz
                      ) {
        if (Int::isZero(a1) || Int::isZero(n)) {
            *nx = Int(0ll);
            *ny = Int(0ll);
            *nz = Int(1ll);
            return;
        }
        if (Int::isOne(n)) {
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
        Int t0 = *nx, t1 = *ny, t2 = *nz;
        fastDouble(t0, t1, t2, nx, ny, nz);
        if (isOdd) {
            Int r0 = *nx, r1 = *ny, r2 = *nz;
            fastAdd(r0, r1, r2, a0, a1, a2, nx, ny, nz);
        }
    }

    const char *getVStr(long v) {
        return v == 0 ? "00" : "01";
    }

    void concatBytes(const char *a, int al, const char *b, int bl, 
                             const char *c, int cl, const char *d, int dl,
                             char *out) {
        for (int i = 0; i < al; ++i)
            out[i] = a[i];
        for (int i = al; i < al + bl; ++i)
            out[i] = b[i - al];
        for (int i = al + bl; i < al + bl + cl; ++i)
            out[i] = c[i - al - bl];
        for (int i = al + bl + cl; i < al + bl + cl + dl; ++i)
            out[i] = d[i - al - bl - cl];
    }

    string sign(string &privateKeyHex, string &hashHex) {
        char v0[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        char k0[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        char char0 = 0, char1 = 1;
        char privateKey[32];
        Int::hexStrToBytes(privateKeyHex, privateKey);
        char hash[32];
        Int::hexStrToBytes(hashHex, hash);
        char k1_in[32 + 1 + 32 + 32];
        concatBytes(v0, 32, &char0, 1, privateKey, 32, hash, 32, k1_in);
        const char *k1 = Hmac::sha256(k0, 32, k1_in, 97);
        const char *v1 = Hmac::sha256(k1, 32, v0, 32);
        char k2_in[32 + 1 + 32 + 32];
        concatBytes(v1, 32, &char1, 1, privateKey, 32, hash, 32, k2_in);
        const char *k2 = Hmac::sha256(k1, 32, k2_in, 97);
        const char *v2 = Hmac::sha256(k2, 32, v1, 32);
        const char *kv = Hmac::sha256(k2, 32, v2, 32);

        string kvHex;
        Int::bytesToHexStr(kv, 32, kvHex);
        Int k(kvHex.c_str(), 16), priv(privateKeyHex.c_str(), 16), hashNum(hashHex.c_str(), 16);

        Int _1 = Int(1ll), _2 = Int(2ll);

        Int pa, pb, pc, kTmp = k;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, kTmp, &pa, &pb, &pc);
        Int z = inv(pc, SECP256K1_P);
        Int r = z.pow(2).mul(pa).mod(SECP256K1_P), y = z.pow(3).mul(pb).mod(SECP256K1_P);

        Int rp = r.mul(priv).add(hashNum);
        Int sRaw = inv(k, SECP256K1_N).mul(rp).mod(SECP256K1_N);
        Int s = sRaw.mul(_2).compareTo(SECP256K1_N) < 0 ? sRaw : SECP256K1_N.sub(sRaw);

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
    bool verify(string &publicKeyHex, string &hashHex, string &sig) {
        int sigLen = sig.length();
        if (sigLen != 130)
            throw runtime_error("Invalid signature, " + sig);

        Int _0 = Int(0ll), _1 = Int(1ll);
        Int r = Int(sig.substr(0, 64).c_str(), 16), s = Int(sig.substr(64, 64).c_str(), 16);
        Int rmodn = r.mod(SECP256K1_N), smodn = s.mod(SECP256K1_N);
        if (Int::isZero(rmodn) || Int::isZero(smodn))
            return false;

        Int x(publicKeyHex.substr(0, 64).c_str(), 16), y(publicKeyHex.substr(64, 64).c_str(), 16);

        Int w = inv(s, SECP256K1_N);
        Int z(hashHex.c_str(), 16);

        Int u1 = z.mul(w).mod(SECP256K1_N), u2 = r.mul(w).mod(SECP256K1_N);

        Int pa, pb, pc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, u1, &pa, &pb, &pc);

        Int pz = inv(pc, SECP256K1_P);
        Int px = pz.pow(2).mul(pa).mod(SECP256K1_P), py = pz.pow(3).mul(pb).mod(SECP256K1_P);

        fastMultiply(x, y, _1, u2, &pa, &pb, &pc);
        Int qz = inv(pc, SECP256K1_P);
        Int qx = qz.pow(2).mul(pa).mod(SECP256K1_P), qy = qz.pow(3).mul(pb).mod(SECP256K1_P);

        fastAdd(px, py, _1, qx, qy, _1, &pa, &pb, &pc);
        Int gz = inv(pc, SECP256K1_P);
        Int gx = gz.pow(2).mul(pa).mod(SECP256K1_P);

        return r.equal(gx);
    }


/**
 * @param privKeyBytes private key content bytes.
 *
 * @return bytes calculate public key bytes from private key bytes.
 * */
    string privateKeyToPublicKeyHex(string &privateKeyHex) {
        Int priv(privateKeyHex.c_str(), 16);
        if (priv.compareTo(SECP256K1_N) > 0)
            throw runtime_error("Invalid private key.");

        Int _0 = Int(0ll), _1 = Int(1ll);

        Int pa, pb, pc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, priv, &pa, &pb, &pc);

        Int z = inv(pc, SECP256K1_P);
        Int x = z.pow(2).mul(pa).mod(SECP256K1_P), y = z.pow(3).mul(pb).mod(SECP256K1_P);
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
    string recoverToPublicKeyHex(const string &sig, string &hashHex) {
        int sigLen = sig.length();
        if (sigLen != 130)
            throw runtime_error("Invalid signature, " + sig);
        Int _27 = Int("27", 10), _34 = Int("34", 10);
        Int r(sig.substr(0, 64).c_str(), 16), s(sig.substr(64, 64).c_str(), 16);

        Int rmodN = r.mod(SECP256K1_N), smodN = s.mod(SECP256K1_N);
        if (Int::isZero(rmodN) || Int::isZero(smodN))
            throw runtime_error("Invalid signature, " + sig);

        Int v(sig.substr(128, 2).c_str(), 16);
        v = v.add(_27);
        if (v.compareTo(_27) < 0 || v.compareTo(_34) > 0)
            throw runtime_error("Invalid signature, " + sig);

        Int x = r;
        Int xAB = x.mul(SECP256K1_A).add(SECP256K1_B);
        Int num = x.pow(3).add(xAB).mod(SECP256K1_P);

        Int _1 = Int(1ll), _2 = Int(2ll), _4 = Int(4ll);
        Int powP = SECP256K1_P.add(_1).div(_4);
        Int y = num.pow(powP, SECP256K1_P);
        long yMod2 = y.isOdd(), vMod2 = v.isOdd();
        if (yMod2 ^ vMod2 == 0)
            y = SECP256K1_P.sub(y);

        Int ynp = y.pow(2).sub(num).mod(SECP256K1_P);
        if (!Int::isZero(ynp))
            throw runtime_error("Invalid signature, " + sig);

        Int z(hashHex.c_str(), 16);
        Int NzN = SECP256K1_N.sub(z).mod(SECP256K1_N);
        Int ga, gb, gc;
        Int xya, xyb, xyc;
        fastMultiply(SECP256K1_Gx, SECP256K1_Gy, _1, NzN, &ga, &gb, &gc);

        fastMultiply(x, y, _1, s, &xya, &xyb, &xyc);

        Int qzx, qzy, qzz;
        fastAdd(ga, gb, gc, xya, xyb, xyc, &qzx, &qzy, &qzz);
        Int QRZ = inv(r, SECP256K1_N);
        Int qx, qy, qz;
        fastMultiply(qzx, qzy, qzz, QRZ, &qx, &qy, &qz);
        Int pubZ = inv(qz, SECP256K1_P);
        Int left = pubZ.pow(2).mul(qx).mod(SECP256K1_P), right = pubZ.pow(3).mul(qy).mod(SECP256K1_P);

        string leftStr = left.toString(16), rightStr = right.toString(16);
        while (leftStr.length() < 64)
            leftStr = "0" + leftStr;
        while (rightStr.length() < 64)
            rightStr = "0" + rightStr;
        return leftStr + rightStr;
    }
}