#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/provider.h>
#include <utility>
#include <iostream>

static int check(int value, const char* msg = nullptr){
    if(value < 0){
        perror(msg ?: "");
        exit(-1);
    }
    return value;
}

static auto check(auto* value, const char* msg = nullptr){
    if(!value){
        perror(msg ?: "");
        exit(-1);
    }
    return value;
}

struct OsslCtx{
    EVP_CIPHER_CTX * ctx;

    explicit OsslCtx(EVP_CIPHER_CTX * ctx): ctx(ctx){};
    ~OsslCtx()
    {
        if(ctx) EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)ctx);
    }
    OsslCtx(const OsslCtx&) = delete;
    OsslCtx(OsslCtx&& other) noexcept
    {
        this->ctx = std::exchange(other.ctx, nullptr);
    }

    int encrypt(unsigned char* data, unsigned char* out, int size){
        int result = 0;
        check(EVP_EncryptUpdate(ctx, out, &result, data, size), "encrypt");
        return result;
    }

    int final_encrypt(unsigned char* data){
        int result = 0;
        check(EVP_EncryptFinal(ctx, data, &result), "encryptf");
        return result;
    }
};

class CtxFactory
{
private:
    OSSL_LIB_CTX* ossl_ctx;
    OSSL_PROVIDER * prov;
    EVP_CIPHER* cipher;
public:
    CtxFactory() = default;

    CtxFactory(const char* provider, const char* cipher_name){
        ossl_ctx = check(OSSL_LIB_CTX_new(), "OSSL_LIB_CTX");
        prov = check(OSSL_PROVIDER_load(NULL, provider), "prov load");
        cipher = check(EVP_CIPHER_fetch(NULL, cipher_name, nullptr), "fetch");
    }

    OsslCtx next(unsigned char* key, unsigned char* iv){
        auto ctx= check(EVP_CIPHER_CTX_new(), "ctx new");
        check(EVP_EncryptInit(ctx, cipher, key, iv), "Enc init");
        return OsslCtx(ctx);
    }
};