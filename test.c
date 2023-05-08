#include "alg.h"
#include "cal.h"
#include <time.h>
#include <stdio.h>

void privPubTest() {
    printf("****begin priv pub test****\n");
    const char *priv_key_hex = "062ac77cfcfe801ca274701dd74d97b39a6210028fa3d7c884f7731e69f167c0";
    uint8_t priv_key[32];
    char pub_key_hex[129] = {'\0'};
    calHexToBytes(priv_key_hex, 64, priv_key);
    sm2PrivateKeyToPublicKey(priv_key_hex, pub_key_hex);
    printf("pubHex = %s\n", pub_key_hex);
}



void testHmacSha256() {
    const char *msg = "nihao,shijie";
    uint8_t ret[32]; 
    char hex[65] = {'\0'};
    sha256((uint8_t*)msg, 12, ret);
    calBytesToHex(ret, 32, hex);
    printf("hex = %s\n", hex);


    const char *key = "123456789012345678901234567890123456789012345678901234567890123";
    hmacSha256((uint8_t*)key, 63, (uint8_t*)msg, 12, ret);
    calBytesToHex(ret, 32, hex);
    printf("hex = %s\n", hex);
}

void sm3Test() {
    printf("****begin sm3 test****\n");

    const char *content_hex = "008031323334353637383132333435363738fffffffeffffffffffffffffffffffffffffffff00000000fffffffffffffffc28e9fa9e9d9f5e344d5a9e4bcf6509a7f39789f515ab8f92ddbcbd414d940e9332c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a07297d04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197f594c30c8cefd2147dc18ace2a77cafa102a8f42696a89483f85fabd387cf7a6";
    const int len = 420;

    uint8_t content[len/2], hash[32];
    char hash_hex[65] = {'\0'};
    calHexToBytes(content_hex, len, content);
    sm3(content, len/2, hash);
    calBytesToHex(hash, 32, hash_hex);
    printf("hash = %s\n", hash_hex);
}

void signTest() {
    printf("****begin sign test****\n");

    const char *priv_key_hex = "062ac77cfcfe801ca274701dd74d97b39a6210028fa3d7c884f7731e69f167c0";
    uint8_t priv_key[32];
    char sig_hex[129] = {'\0'};
    calHexToBytes(priv_key_hex, 64, priv_key);
    sm2Sign(priv_key_hex, priv_key, 32, sig_hex);
    printf("sig = %s\n", sig_hex);
}

void verifyTest() {
    const char *sig_hex = "e618993a543cfa228f0c9b71159cb03a98a6b7f201b3bc776c65a82f88391d91aac52d14aef139958b4b1deb1c6989f04d5f619ccb9c2b1e1a66abb8c90d81a8";
    const char *pub_key_hex = "585979996184e9d652dfc7aa90c3cf94ff6df42dcecd478e9fc36f3d58aeb08e8f091044a62869b22cca6729fd3266b35301c947e63ebfaacd8728a92ff7627a";
    const char *content_hex = "31a7048b7240a0fa29148cef77b5cc023588997c1cc23fa4d798ecd84ec520ea";
    uint8_t content[32];
    calHexToBytes(content_hex, 64, content);
    int ret = sm2Verify(pub_key_hex, content, 32, sig_hex);
    printf("verify ret = %s\n", ret?"true":"false");
}

void ecPrivPubTest() {
    printf("****begin ec priv pub test****\n");

    const char *priv_key_hex = "7297d04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197";
    uint8_t priv_key[32];
    char pub_key_hex[129] = {'\0'};
    calHexToBytes(priv_key_hex, 64, priv_key);
    ecPrivateKeyToPublicKey(priv_key_hex, pub_key_hex);
    printf("pubHex = %s\n", pub_key_hex);
}


void ecSignTest() {
    printf("****begin ec sign test****\n");
    const char *priv_key_hex = "7297d04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197";
    const char *msg_hex = "ffffd04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197";
    uint8_t msg[32];
    char sig_hex[131] = {'\0'};
    calHexToBytes(msg_hex, 64, msg);
    if(!ecSign(priv_key_hex, msg, 32, sig_hex)) {
        printf("sign failed.\n");
    } else {
        printf("sig = %s\n", sig_hex);
    }
}


void ecVerifyTest() {
    const char *sig_hex = "e82c473c924af6828c4ecbf2277373412544a344f57e0b3ac98a3083224cb498acc3a4bc54246bbcd6496b4eca0180a6c5c4d92bd8a5390d7144bf50cb1e992500";
    const char *pub_key_hex = "4ac1846cf62d3d1386ed7c26f0f251457adbd2f89466df9e37421c8d743e560665f027be426847215d34cbeac367031fc906a89daa81978d8d57ff3eb3a288bf";
    const char *content_hex = "7297d04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197";
    uint8_t content[32];
    calHexToBytes(content_hex, 64, content);
    int ret = ecVerify(pub_key_hex, content, 32, sig_hex);
    printf("verify ret = %s\n", ret?"true":"false");
}

void ecRecoverTest() {
    const char *sig_hex = "3fbd52b3cc4de31fa55424c4addb7e39037317db299b009db8c4b1a506c76ba75660cf33316c015a8d9442c42daa709cb07fd02626e9b27b16943d977b391c0a01";
    const char *pub_key_hex_ori = "4ac1846cf62d3d1386ed7c26f0f251457adbd2f89466df9e37421c8d743e560665f027be426847215d34cbeac367031fc906a89daa81978d8d57ff3eb3a288bf";
    const char *content_hex = "7297d04a21342d8055111fe1c57e7626c8837e83f88301ad35e91b5f9f9d8197";
    char pub_key_hex[129] = {0};
    pub_key_hex[128] = '\0';
    uint8_t content[32];
    calHexToBytes(content_hex, 64, content);
    if(ecRecoverToPublicKey(content, 32, sig_hex, pub_key_hex)) {
        printf("pub_key_hex = %s\n", pub_key_hex);
    } else {
        printf("recover failed.\n");
    }
}

// windows
// gcc -fPIC test.c sm2p256v1.c sm3.c cal.c keccak256.c secp256k1.c hmac_sha256.c -lgmp -o test.exe
// linux
// gcc -fPIC test.c sm2p256v1.c sm3.c cal.c keccak256.c secp256k1.c hmac_sha256.c -lgmp -o test_bin

int main() {
    printf("start now.\n");
    testHmacSha256();
    sm3Test();

    privPubTest();
    signTest();
    verifyTest();

    ecPrivPubTest();
    ecSignTest();
    ecVerifyTest();
    ecRecoverTest();

    return 1;
}