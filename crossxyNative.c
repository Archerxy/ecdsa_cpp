#include <jni.h>
#include "alg.h"

#ifndef _Included_com_whty_cross_core_utils_algorithm_CrossxyNative
#define _Included_com_whty_cross_core_utils_algorithm_CrossxyNative
#ifdef __cplusplus
extern "C" {
#endif


/*  
* gcc -c crossxyNative.c -L. -lxy_alg_win32  
* ar -cr libcrossxy_alg_win.a *.o
*
* # Linux
* gcc -fPIC -shared sm2p256v1.c sm3.c cal.c keccak256.c secp256k1.c hmac_sha256.c crossxyNative.c -lgmp -o libcrossxy_alg_linux.so -O3 -static-libgcc -static-libstdc++ --std=c99
* 
* # Windows
* gcc -fPIC -c ../sm2p256v1.c ../sm3.c ../cal.c ../keccak256.c ../secp256k1.c ../hmac_sha256.c ../crossxyNative.c -O3 -static-libgcc -static-libstdc++
* ar -x libgmp.a
* gcc -fPIC -shared *.o -static-libgcc -static-libstdc++ -o libcrossxy_alg_win.dll
*/

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    sm3
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_algorithm_AlgCollection_sm3
  (JNIEnv *env, jclass clazz, jbyteArray j_content) {
    if(NULL == j_content) {
        return NULL;
    }
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t len = (*env)->GetArrayLength(env, j_content);
    uint8_t hash[32];
    jbyteArray ret = NULL;
    if(!sm3(content, len, hash)) {
        goto END;
    }
	ret = (*env)->NewByteArray(env, 32);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, 32, (jbyte *)hash);
    }

END: 
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    sm2PrivateKeyToPublicKey
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_algorithm_AlgCollection_sm2PrivateKeyToPublicKey
  (JNIEnv *env, jclass clazz, jstring j_pri_key_hex) {
    if(NULL == j_pri_key_hex) {
        return NULL;
    }
    const char *pri_key_hex = (*env)->GetStringUTFChars(env, j_pri_key_hex, NULL);
    char pub_key_hex[129];
    jstring ret = NULL;
    pub_key_hex[128] = '\0';
    if(!sm2PrivateKeyToPublicKey(pri_key_hex, pub_key_hex)) {
        goto END;
    }
    ret = (*env)->NewStringUTF(env, pub_key_hex);

END: 
    if(pri_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pri_key_hex, pri_key_hex);
    }

    return ret;
  }
  
/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    sm2Sign
 * Signature: (Ljava/lang/String;[B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_algorithm_AlgCollection_sm2Sign
  (JNIEnv *env, jclass clazz, jstring j_pri_key_hex, jbyteArray j_content) {
    if(NULL == j_pri_key_hex || NULL == j_content) {
        return NULL;
    }
    const char *pri_key_hex = (*env)->GetStringUTFChars(env, j_pri_key_hex, NULL);
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t content_len = (*env)->GetArrayLength(env, j_content);
    char sig_hex[129];
    jstring ret = NULL;
    sig_hex[128] = '\0';
    if(!sm2Sign(pri_key_hex, content, content_len, sig_hex)) {
        goto END;
    }
    ret = (*env)->NewStringUTF(env, sig_hex);
    
END: 
    if(pri_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pri_key_hex, pri_key_hex);
    }
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    sm2Verify
 * Signature: (Ljava/lang/String;[BLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_algorithm_AlgCollection_sm2Verify
  (JNIEnv *env, jclass clazz, jstring j_pub_key_hex, jbyteArray j_content, jstring j_sig_hex) {
    if(NULL == j_pub_key_hex || NULL == j_content || NULL == j_sig_hex) {
        return JNI_FALSE;
    }
    const char *pub_key_hex = (*env)->GetStringUTFChars(env, j_pub_key_hex, NULL);
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t content_len = (*env)->GetArrayLength(env, j_content);
    const char *sig_hex = (*env)->GetStringUTFChars(env, j_sig_hex, NULL);
    int ret = sm2Verify(pub_key_hex, content, content_len, sig_hex);

    if(pub_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pub_key_hex, pub_key_hex);
    }
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    if(sig_hex) {
        (*env)->ReleaseStringUTFChars(env, j_sig_hex, sig_hex);
    }
    return ret ? JNI_TRUE : JNI_FALSE;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    keccak256
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_algorithm_AlgCollection_keccak256
  (JNIEnv *env, jclass clazz, jbyteArray j_content) {
    if(NULL == j_content) {
        return NULL;
    }
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t len = (*env)->GetArrayLength(env, j_content);
    uint8_t hash[32];
    jbyteArray ret = NULL;
    if(!keccak256(content, len, hash)) {
        goto END;
    }
	ret = (*env)->NewByteArray(env, 32);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, 32, (jbyte *)hash);
    }

END: 
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    sha256
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_algorithm_AlgCollection_sha256
  (JNIEnv *env, jclass clazz, jbyteArray j_content) {
    if(NULL == j_content) {
        return NULL;
    }
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t len = (*env)->GetArrayLength(env, j_content);
    uint8_t hash[32];
    jbyteArray ret = NULL;
    if(!sha256(content, len, hash)) {
        goto END;
    }
	ret = (*env)->NewByteArray(env, 32);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, 32, (jbyte *)hash);
    }

END: 
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    hmacSha256
 * Signature: ([B[B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_algorithm_AlgCollection_hmacSha256
  (JNIEnv *env, jclass clazz, jbyteArray j_key, jbyteArray j_content) {
    if(NULL == j_content) {
        return NULL;
    }
	uint8_t *key = (*env)->GetByteArrayElements(env, j_key, NULL);
	uint32_t key_len = (*env)->GetArrayLength(env, j_key);
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t len = (*env)->GetArrayLength(env, j_content);
    uint8_t hash[32];
    jbyteArray ret = NULL;
    if(!hmacSha256(key, key_len, content, len, hash)) {
        goto END;
    }
	ret = (*env)->NewByteArray(env, 32);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, 32, (jbyte *)hash);
    }

END: 
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    ecPrivateKeyToPublicKey
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_algorithm_AlgCollection_ecPrivateKeyToPublicKey
  (JNIEnv *env, jclass clazz, jstring j_pri_key_hex) {
    if(NULL == j_pri_key_hex) {
        return NULL;
    }
    const char *pri_key_hex = (*env)->GetStringUTFChars(env, j_pri_key_hex, NULL);
    char pub_key_hex[129];
    jstring ret = NULL;
    pub_key_hex[128] = '\0';
    if(!ecPrivateKeyToPublicKey(pri_key_hex, pub_key_hex)) {
        goto END;
    }
    ret = (*env)->NewStringUTF(env, pub_key_hex);

END: 
    if(pri_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pri_key_hex, pri_key_hex);
    }

    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    ecSign
 * Signature: (Ljava/lang/String;[B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_algorithm_AlgCollection_ecSign
  (JNIEnv *env, jclass clazz, jstring j_pri_key_hex, jbyteArray j_content) {
    if(NULL == j_pri_key_hex || NULL == j_content) {
        return NULL;
    }
    const char *pri_key_hex = (*env)->GetStringUTFChars(env, j_pri_key_hex, NULL);
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t content_len = (*env)->GetArrayLength(env, j_content);
    char sig_hex[131];
    jstring ret = NULL;
    sig_hex[130] = '\0';
    if(!ecSign(pri_key_hex, content, content_len, sig_hex)) {
        goto END;
    }
    ret = (*env)->NewStringUTF(env, sig_hex);
    
END: 
    if(pri_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pri_key_hex, pri_key_hex);
    }
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    return ret;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    ecVerify
 * Signature: (Ljava/lang/String;[BLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_algorithm_AlgCollection_ecVerify
  (JNIEnv *env, jclass clazz, jstring j_pub_key_hex, jbyteArray j_content, jstring j_sig_hex) {
    if(NULL == j_pub_key_hex || NULL == j_content || NULL == j_sig_hex) {
        return JNI_FALSE;
    }
    const char *pub_key_hex = (*env)->GetStringUTFChars(env, j_pub_key_hex, NULL);
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t content_len = (*env)->GetArrayLength(env, j_content);
    const char *sig_hex = (*env)->GetStringUTFChars(env, j_sig_hex, NULL);
    int ret = ecVerify(pub_key_hex, content, content_len, sig_hex);

    if(pub_key_hex) {
        (*env)->ReleaseStringUTFChars(env, j_pub_key_hex, pub_key_hex);
    }
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    if(sig_hex) {
        (*env)->ReleaseStringUTFChars(env, j_sig_hex, sig_hex);
    }
    return ret ? JNI_TRUE : JNI_FALSE;
  }

/*
 * Class:     com_whty_cross_core_utils_algorithm_CrossxyNative
 * Method:    ecRecoverToPublicKey
 * Signature: ([BLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_algorithm_AlgCollection_ecRecoverToPublicKey
  (JNIEnv *env, jclass clazz, jbyteArray j_content, jstring j_sig_hex) {
    if(NULL == j_content || NULL == j_sig_hex) {
        return NULL;
    }
	uint8_t *content = (*env)->GetByteArrayElements(env, j_content, NULL);
	uint32_t content_len = (*env)->GetArrayLength(env, j_content);
    const char *sig_hex = (*env)->GetStringUTFChars(env, j_sig_hex, NULL);
    char pub_key_hex[129];
    jstring ret = NULL;
    pub_key_hex[128] = '\0';
    if(!ecRecoverToPublicKey(content, content_len, sig_hex, pub_key_hex)) {
        goto END;
    }
    ret = (*env)->NewStringUTF(env, pub_key_hex);

END: 
    if(content) {
        (*env)->ReleaseByteArrayElements(env, j_content, content, 0);
    }
    if(sig_hex) {
        (*env)->ReleaseStringUTFChars(env, j_sig_hex, sig_hex);
    }
    return ret;
  }

#ifdef __cplusplus
}
#endif
#endif
