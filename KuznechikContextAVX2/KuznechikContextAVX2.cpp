#include "KuznechikContextAVX2.hpp"
#include <iostream>

extern const OSSL_DISPATCH kuznechik2Functions[] = 
{
    { OSSL_FUNC_CIPHER_NEWCTX, (void (*)(void))kuznechik2NewCtx },
    { OSSL_FUNC_CIPHER_FREECTX, (void (*)(void))kuznechik2FreeCtx},
    { OSSL_FUNC_CIPHER_ENCRYPT_INIT, (void (*)(void))kuznechik2OperationInit },
    { OSSL_FUNC_CIPHER_DECRYPT_INIT, (void (*)(void))kuznechik2OperationInit },
    { OSSL_FUNC_CIPHER_UPDATE, (void (*)(void))kuznechik2Update },
    { OSSL_FUNC_CIPHER_FINAL, (void (*)(void))kuznechik2Final },
    { OSSL_FUNC_CIPHER_GET_PARAMS, (void (*)(void))kuznechik2GetParams },
    { OSSL_FUNC_CIPHER_GETTABLE_PARAMS, (void (*)(void))kuznechik2GetTableParams },
    { OSSL_FUNC_CIPHER_GET_CTX_PARAMS, (void (*)(void))kuznechik2GetCtxParams },
    { OSSL_FUNC_CIPHER_GETTABLE_CTX_PARAMS, (void (*)(void))kuznechik2GetTableCtxParams },
    { OSSL_FUNC_CIPHER_SET_CTX_PARAMS, (void (*)(void))kuznechik2SetCtxParams },
    { OSSL_FUNC_CIPHER_SETTABLE_CTX_PARAMS, (void (*)(void))kuznechik2SetTableCtxParams },
    { 0, NULL }
};

static void *kuznechik2NewCtx(void *provCtx)
{
    struct kuznechik2CtxSt *ctx = new kuznechik2CtxSt;
    if (ctx != nullptr)
    {
        std::memset(ctx, 0, sizeof(*ctx));
        ctx->provCtx = (provider_ctx_st*)provCtx;
        ctx->keyl = KEY_LENGTH_KUZ_AVX2;
        ctx->sizeBlock = BLOCK_SIZE_KUZ_AVX2;
        ctx->buffer2.resize(BLOCK_SIZE_KUZ_AVX2 / sizeof(byteVectorKuznechik));
        ctx->ivl = IV_LENGTH_KUZ_AVX2;
    }
    return ctx;
}

static void kuznechik2FreeCtx(void *kuznechik2Ctx)
{
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
    ctx->provCtx = nullptr;
    delete ctx;
}

static int kuznechik2OperationInit(void *kuznechik2Ctx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[])
{
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
    if (k != nullptr)
    {
        delete ctx->key;
        ctx->key = new unsigned char[ctx->keyl];
        std::copy_n(k, ctx->keyl, ctx->key);

        delete ctx->iv;
        ctx->iv = new unsigned char[ivlen];
        std::copy_n(iv, ivlen, ctx->iv);

        ctx->ivu = *reinterpret_cast<const uint64_t*>(iv);

        ctx->keyl = keyl;
        ctx->ivl = ivlen;
        ctx->K.changeKey(key((uint8_t*)ctx->key));
    }

    return 1;
}
    
static int kuznechik2Update(void *kuznechik2Ctx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl)
{
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
    size_t sizeBlock = ctx->sizeBlock;
    size_t processed = 0;
    size_t *partialBlockLen = &(ctx->partialBlockLen);
    std::vector<byteVectorKuznechik> result(BLOCK_SIZE_KUZ_AVX2 / sizeof(byteVectorKuznechik));
   
    *partialBlockLen += inl;
    for (size_t i = 0; i < inl / BLOCK_SIZE_KUZ_AVX2; ++i)
    {
        std::copy_n(in + i * BLOCK_SIZE_KUZ_AVX2, BLOCK_SIZE_KUZ_AVX2, (unsigned char*)&ctx->buffer2[0]);
        ctx->K.processDataGamma(ctx->buffer2, result, ctx->ivu);
        ctx->ivu += 0x02;
        std::copy_n((unsigned char*)&result[0], BLOCK_SIZE_KUZ_AVX2, out + i * BLOCK_SIZE_KUZ_AVX2);
        processed += sizeBlock;
        ctx->partialBlockLen -= sizeBlock;
        ctx->last += sizeBlock;
    }

    std::copy_n(in + processed, inl % BLOCK_SIZE_KUZ_AVX2, (unsigned char*)&ctx->buffer2[0]);      
    *outl = processed;

    return 1;
}

static int kuznechik2Final(void *kuznechik2Ctx, unsigned char *out, size_t *outl, size_t outsize)
{
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
    size_t sizeBlock = ctx->sizeBlock;
    size_t partialBlockLen = ctx->partialBlockLen;
    std::vector<byteVectorKuznechik> result(BLOCK_SIZE_KUZ_AVX2 / sizeof(byteVectorKuznechik));
    ctx->K.processDataGamma(ctx->buffer2, result, ctx->ivu);
    ctx->ivu += 0x02;
    std::copy_n((unsigned char*)&result[0], partialBlockLen, out);
    *outl = partialBlockLen;
    
    return 1;    
}

static const OSSL_PARAM *kuznechik2GetTableParams(void *provCtx)
{
    static const OSSL_PARAM table[] = 
    {
        { "blocksize", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t),  0},
        { "keylen", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t),  0},
        { "ivlen", OSSL_PARAM_UNSIGNED_INTEGER, NULL, sizeof(size_t), 0 },
        { NULL, 0, NULL, 0, 0 },
    };
        
    return table;
}

static int kuznechik2GetParams(OSSL_PARAM params[])
{
    OSSL_PARAM *p;
    int ok = 1;

    for (p = params; p->key != NULL; p++)
    {
        switch (gostParamsParse(p->key)) 
    {
    case V_PARAM_blocksize:
        ok &= provnum_set_size_t(p, BLOCK_SIZE_KUZ_AVX2) >= 0;
        break;
    case V_PARAM_keylen:
        ok &= provnum_set_size_t(p, KEY_LENGTH_KUZ_AVX2) >= 0;
        break;
    case V_PARAM_ivlen:
        ok &= provnum_set_size_t(p, IV_LENGTH_KUZ_AVX2) >= 0;
        break;
    }
    }
    return ok;
} 

static const OSSL_PARAM *kuznechik2GetTableCtxParams(void *cCtx, void *provCtx)
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

static const OSSL_PARAM *kuznechik2SetTableCtxParams(void *cCtx, void *provCtx)
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
    
static int kuznechik2GetCtxParams(void *kuznechik2Ctx, OSSL_PARAM params[])
{
        
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
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
    
static int kuznechik2SetCtxParams(void *kuznechik2Ctx, const OSSL_PARAM params[])
{  
    struct kuznechik2CtxSt *ctx = (kuznechik2CtxSt*)kuznechik2Ctx;
    const OSSL_PARAM *p;

    if ((p = OSSL_PARAM_locate_const(params, "key")) != NULL) {
        unsigned char key[KEY_LENGTH_KUZ_AVX2];
        size_t keylen = sizeof(key);
        memcpy(ctx->key, key, sizeof(ctx->key));
    }

    if ((p = OSSL_PARAM_locate_const(params, "iv")) != NULL) {
        unsigned char iv[IV_LENGTH_KUZ_AVX2];
        size_t ivlen = sizeof(iv);
        memcpy(ctx->iv, iv, sizeof(ctx->iv));
    }

    return 1; 
}
    

    
    

    

