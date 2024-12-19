#include "customProvider.hpp"
#include "g_params.hpp"
#include "prov/num.h"
#include <string>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>

static void providerCtxFree(struct provider_ctx_st *ctx)
{
    delete ctx;
}

static struct provider_ctx_st* providerCtxNew(const OSSL_CORE_HANDLE *handle, const OSSL_DISPATCH *in)
{
    struct provider_ctx_st *ctx;
    ctx = new provider_ctx_st;

    if (ctx != nullptr)
    {
        ctx->handle = (OSSL_CORE_HANDLE *)handle;
    } 
    else
    {
        providerCtxFree(ctx);
        ctx = nullptr;
    }
        
    return ctx;
}

int OSSL_provider_init(const OSSL_CORE_HANDLE *core, const OSSL_DISPATCH *in, const OSSL_DISPATCH **out, void **vprovctx)
{
    if ((*vprovctx = providerCtxNew(core, in)) == nullptr)
    {
        return 0;
    }  
    *out = providerFunctions;
    return 1;
}

static void gostProvTeardown(void *providerCtx)
{
    providerCtxFree((provider_ctx_st*)providerCtx);
}

static const OSSL_ALGORITHM *gostProvOperation(void *providerCtx, int operationId, int *noCache)
{
    *noCache = 0;
    switch (operationId) 
    {
        case OSSL_OP_CIPHER:
            return ghostCiphersTable;
    }
    return NULL;
}

/*
static int gostProviderGetParams(void *providerCtx, OSSL_PARAM *params)
{
    OSSL_PARAM *p;
    int ok = 1;

    /*
    добавить реализацию
    
    return ok;
}
*/