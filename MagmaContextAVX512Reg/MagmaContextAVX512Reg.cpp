#include "MagmaContextAVX512Reg.hpp"
#include <iostream>

extern const OSSL_DISPATCH magma512RegFunctions[] = 
{
    { OSSL_FUNC_CIPHER_NEWCTX, (void (*)(void))magma512RegNewCtx },
    { OSSL_FUNC_CIPHER_FREECTX, (void (*)(void))magma512RegFreeCtx},
    { OSSL_FUNC_CIPHER_ENCRYPT_INIT, (void (*)(void))magma512RegOperationInit },
    { OSSL_FUNC_CIPHER_DECRYPT_INIT, (void (*)(void))magma512RegOperationInit },
    { OSSL_FUNC_CIPHER_UPDATE, (void (*)(void))magma512RegUpdate },
    { OSSL_FUNC_CIPHER_FINAL, (void (*)(void))magma512RegFinal },
    { OSSL_FUNC_CIPHER_GET_PARAMS, (void (*)(void))magma512RegGetParams },
    { OSSL_FUNC_CIPHER_GETTABLE_PARAMS, (void (*)(void))magma512RegGetTableParams },
    { OSSL_FUNC_CIPHER_GET_CTX_PARAMS, (void (*)(void))magma512RegGetCtxParams },
    { OSSL_FUNC_CIPHER_GETTABLE_CTX_PARAMS, (void (*)(void))magma512RegGetTableCtxParams },
    { OSSL_FUNC_CIPHER_SET_CTX_PARAMS, (void (*)(void))magma512RegSetCtxParams },
    { OSSL_FUNC_CIPHER_SETTABLE_CTX_PARAMS, (void (*)(void))magma512RegSetTableCtxParams },
    { 0, NULL }
};

static void *magma512RegNewCtx(void *provCtx)
{
    struct magma512RegCtxSt *ctx = new magma512RegCtxSt;
    if (ctx != nullptr)
    {
        std::memset(ctx, 0, sizeof(*ctx));
        ctx->provCtx = (provider_ctx_st*)provCtx;
        ctx->keyl = KEY_LENGTH_MAG_AVX512Reg;
        ctx->sizeBlock = BLOCK_SIZE_MAG_AVX512Reg;
        ctx->buffer2.resize(BLOCK_SIZE_MAG_AVX512Reg / sizeof(byteVectorMagma));
        ctx->ivl = IV_LENGTH_MAG_AVX512Reg;
    }
    return ctx;
}

static void magma512RegFreeCtx(void *magma512RegCtx)
{
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    ctx->provCtx = nullptr;
    delete ctx;
}

static int magma512RegOperationInit(void *magma512RegCtx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[])
{
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    if (k != nullptr)
    {
        delete ctx->key;
        ctx->key = new unsigned char[ctx->keyl];
        std::copy_n(k, ctx->keyl, ctx->key);

        delete ctx->iv;
        ctx->iv = new unsigned char[ivlen];
        std::copy_n(iv, ivlen, ctx->iv);

        ctx->ivu = *reinterpret_cast<const uint32_t*>(iv);

        ctx->keyl = keyl;
        ctx->ivl = ivlen;
        ctx->M.changeKey(key((uint8_t*)ctx->key));
    }

    return 1;
}
    
static int magma512RegUpdate(void *magma512RegCtx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl)
{
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    size_t sizeBlock = ctx->sizeBlock;
    size_t processed = 0;
    size_t *partialBlockLen = &(ctx->partialBlockLen);
    std::vector<byteVectorMagma> result(BLOCK_SIZE_MAG_AVX512Reg / sizeof(byteVectorMagma));
   
    *partialBlockLen += inl;
    for (size_t i = 0; i < inl / BLOCK_SIZE_MAG_AVX512Reg; ++i)
    {
        std::copy_n(in + i * BLOCK_SIZE_MAG_AVX512Reg, BLOCK_SIZE_MAG_AVX512Reg, (unsigned char*)&ctx->buffer2[0]);
        ctx->M.processDataGamma(ctx->buffer2, result, ctx->ivu);
        ctx->ivu += 0x08;
        std::copy_n((unsigned char*)&result[0], BLOCK_SIZE_MAG_AVX512Reg, out + i * BLOCK_SIZE_MAG_AVX512Reg);
        processed += sizeBlock;
        ctx->partialBlockLen -= sizeBlock;
        ctx->last += sizeBlock;
    }

    std::copy_n(in + processed, inl % BLOCK_SIZE_MAG_AVX512Reg, (unsigned char*)&ctx->buffer2[0]);      
    *outl = processed;

    return 1;
}

static int magma512RegFinal(void *magma512RegCtx, unsigned char *out, size_t *outl, size_t outsize)
{
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    size_t sizeBlock = ctx->sizeBlock;
    size_t partialBlockLen = ctx->partialBlockLen;
    std::vector<byteVectorMagma> result(BLOCK_SIZE_MAG_AVX512Reg / sizeof(byteVectorMagma));
    ctx->M.processDataGamma(ctx->buffer2, result, ctx->ivu);
    ctx->ivu += 0x04;
    std::copy_n((unsigned char*)&result[0], partialBlockLen, out);
    *outl = partialBlockLen;

    return 1;    
}

static const OSSL_PARAM *magma512RegGetTableParams(void *provCtx)
{
    static const OSSL_PARAM table[] = 
    {
        { "blocksize", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { "keylen", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { "ivlen", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { NULL, 0, NULL, 0, 0 },
    };
        
    return table;
}

static int magma512RegGetParams(OSSL_PARAM params[])
{
    OSSL_PARAM *p;
    int ok = 1;

    for (p = params; p->key != NULL; p++)
    switch (gostParamsParse(p->key)) 
    {
    case V_PARAM_blocksize:
        ok &= provnum_set_size_t(p, 1) >= 0;
        break;
    case V_PARAM_keylen:
        ok &= provnum_set_size_t(p, KEY_LENGTH_MAG_AVX512Reg) >= 0;
        break;
    case V_PARAM_ivlen:
        ok &= provnum_set_size_t(p, IV_LENGTH_MAG_AVX512Reg) >= 0;
        break;
    }
    return ok;
} 

static const OSSL_PARAM *magma512RegGetTableCtxParams(void *cCtx, void *provCtx)
{
    static const OSSL_PARAM table[] = 
    {
        { S_PARAM_blocksize, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { S_PARAM_keylen, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { S_PARAM_ivlen, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { NULL, 0, NULL, 0, 0 },
    };

    return table;
}   

static const OSSL_PARAM *magma512RegSetTableCtxParams(void *cCtx, void *provCtx)
{
    static const OSSL_PARAM table[] = 
    {
        { S_PARAM_blocksize, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { S_PARAM_keylen, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { S_PARAM_ivlen, OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { NULL, 0, NULL, 0, 0 },
    };
    
    return table;
}
    
static int magma512RegGetCtxParams(void *magma512RegCtx, OSSL_PARAM params[])
{
        
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    int ok = 1;

    if (ctx->keyl > 0) 
    {
        OSSL_PARAM *p;
        for (p = params; p->key != NULL; p++)
            switch (gostParamsParse(p->key)) 
            {
            case V_PARAM_keylen:
                ok &= provnum_set_size_t(p, ctx->keyl) >= 0;
                break;
            }      
    }

    if (ctx->ivl > 0) 
    {
        OSSL_PARAM *p;
        for (p = params; p->key != NULL; p++)
            switch (gostParamsParse(p->key)) 
            {
            case V_PARAM_ivlen:
                ok &= provnum_set_size_t(p, ctx->ivl) >= 0;
                break;
            }      
    }

    return ok;
}
    
static int magma512RegSetCtxParams(void *magma512RegCtx, const OSSL_PARAM params[])
{  
    struct magma512RegCtxSt *ctx = (magma512RegCtxSt*)magma512RegCtx;
    const OSSL_PARAM *p;

    if ((p = OSSL_PARAM_locate_const(params, "key")) != NULL) {
        unsigned char key[KEY_LENGTH_MAG_AVX512Reg];
        size_t keylen = sizeof(key);
        memcpy(ctx->key, key, sizeof(ctx->key));
    }

    if ((p = OSSL_PARAM_locate_const(params, "iv")) != NULL) {
        unsigned char iv[IV_LENGTH_MAG_AVX512Reg];
        size_t ivlen = sizeof(iv);
        memcpy(ctx->iv, iv, sizeof(ctx->iv));
    }

    return 1; 
}
    

    
    

    

