#include <stdint.h>
#include <string.h>
#include "gmp.h"

#ifndef _ALG_H_
#define _ALG_H_

// extern "C" {

/**
* sm3 hash
* @param hex input hex
* @param hex_len hex length
* @param bytes bytes return result
* @return 0=failed 1=succeed 
*/
int calHexToBytes(const char *hex, unsigned int hex_len, uint8_t *bytes);

/**
* sm3 hash
* @param bytes input bytes
* @param bytes_len bytes length
* @param hex hex return result
* @return 0=failed 1=succeed 
*/
int calBytesToHex(uint8_t *bytes, unsigned int bytes_len, char *hex);

/**
* sm3 hash
* @param content input bytes
* @param content_len bytes length
* @param hash bytes return result
* @return 0=failed 1=succeed 
*/
int sm3(uint8_t *content, uint32_t content_len, uint8_t *hash);

/**
* sm2 convert privateKey hex to publicKey hex
* @param priv_key_hex privateKey hex, length = 64 hex char
* @param pub_key_hex publicKey hex, length = 128 hex char
* @return 0=failed 1=succeed 
*/
int sm2PrivateKeyToPublicKey(const char *priv_key_hex, char *pub_key_hex);


/**
* sm2 sign function
* @param priv_key_hex privateKey hex, length = 64 hex char
* @param content input bytes
* @param len content bytes length
* @param sig_hex signature hex, length = 128 hex char
* @return 0=failed 1=succeed
*/
int sm2Sign(const char *priv_key_hex, uint8_t *content, uint32_t len, char *sig_hex);


/**
* sm2 verify function
* @param pub_key_hex publicKey hex, length = 128 hex char
* @param content input bytes
* @param len bytes length
* @param si_hex signature hex, length = 128 hex char
* @return 0=failed 1=succeed
*/
int sm2Verify(const char *pub_key_hex, uint8_t *content, uint32_t len, const char *sig_hex);



/**
* keccak256 hash
* @param content input bytes
* @param content_len bytes length
* @param hash bytes return result
* @return 0=failed 1=succeed 
*/
int keccak256(uint8_t *content, uint32_t content_len, uint8_t *hash);


/**
* sha256 hash
* @param content input bytes
* @param content_len bytes length
* @param hash bytes return result
* @return 0=failed 1=succeed 
*/
int sha256(uint8_t *content, uint32_t content_len, uint8_t *hash);


/**
* hmacSha256 hash
* @param key hmac secret key
* @param key_len key length
* @param content input bytes
* @param content_len bytes length
* @param hash bytes return result
* @return 0=failed 1=succeed 
*/
int hmacSha256(uint8_t *key, uint32_t key_len, uint8_t *content, uint32_t content_len, uint8_t *hash);


/**
* secp256k1 convert privateKey hex to publicKey hex
* @param priv_key_hex privateKey hex, length = 64 hex char
* @param pub_key_hex publicKey hex, length = 128 hex char
* @return 0=failed 1=succeed 
*/
int ecPrivateKeyToPublicKey(const char *priv_key_hex, char *pub_key_hex);

/**
* secp256k1 sign function
* @param priv_key_hex privateKey hex, length = 64 hex char
* @param content input bytes
* @param len content bytes length
* @param sig_hex result, signature hex, length = 130 hex char
* @return 0=failed 1=succeed
*/
int ecSign(const char *priv_key_hex, uint8_t *content, uint32_t len, char *sig_hex);

/**
* secp256k1 verify function
* @param pub_key_hex publicKey hex, length = 128 hex char
* @param content input bytes
* @param len bytes length
* @param si_hex signature hex, length = 130 hex char
* @return 0=failed 1=succeed
*/
int ecVerify(const char *pub_key_hex, uint8_t *content, uint32_t len, const char *sig_hex);

/**
* secp256k1 recover function
* @param content input bytes
* @param len bytes length
* @param si_hex signature hex, length = 130 hex char
* @param pub_key_hex result, publicKey hex, length = 128 hex char
* @return 0=failed 1=succeed
*/
int ecRecoverToPublicKey(uint8_t *content, uint32_t len, const char *sig_hex, char *pub_key_hex);

// }

#endif