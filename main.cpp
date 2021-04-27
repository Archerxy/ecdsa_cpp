#include <iostream>
#include <string>
#include <ctime>

#include "algorithm/Ecdsa.h"
#include "algorithm/EcdsaBigInt.h"


using namespace std;

void ecdsaCompareAndTest() {
    string priKeyHex = "43EFA45ABDA29F5E4A7FEFABC3DACF7ACFF4EAAF861AF5DFFF3AD1F6543F3ACA";
    string hashStr = "FAE432CE3DDAFCE3457FEFABC3DACF7ACFF4EAAF861AF5DFFF3AFACEB56D4AA4";

    clock_t t0, t1, t2, t3, t4, t5;

    t0 = clock();
    string sig = EcdsaBigInt::sign(priKeyHex, hashStr);
    string pubKey = EcdsaBigInt::privateKeyToPublicKey(priKeyHex);
    int flag = EcdsaBigInt::verify(pubKey, hashStr,sig);
    string publicKey = EcdsaBigInt::recoverToPublicKey(sig,hashStr);
    t1 = clock();
    cout<<"sig: "<<sig<<"\n";
    cout<<"pubkey: "<<pubKey<<"\n";
    cout<<"flag: "<<flag<<"\n";
    cout<<"pk: "<<publicKey<<"\n";
    cout<<"cost :"<<t1-t0<<"ms\n";


    t2 = clock();
    string sig2 = Ecdsa::sign(priKeyHex, hashStr);
    string pubKeyBs2 = Ecdsa::privateKeyToPublicKeyHex(priKeyHex);
    int flag2 = Ecdsa::verify(pubKeyBs2,hashStr,sig);
    string pkBs2 = Ecdsa::recoverToPublicKeyHex(sig2,hashStr);
    t3 = clock();
    cout<<"sig: "<<sig2<<"\n";
    cout<<"pubkey: "<<pubKeyBs2<<"\n";
    cout<<"flag: "<<flag2<<"\n";
    cout<<"pk: "<<pkBs2<<"\n";
    cout<<"cost :"<<t3-t2<<"ms\n";
}

int main() {
    ecdsaCompareAndTest();
    return 0;
}
