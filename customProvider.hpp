#pragma once
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <openssl/ssl.h>
#include <algorithm>
#include <climits>
#include <cstring>
#include <iostream>
#include "./MagmaAVX2/MagmaAVX2.hpp"
#include <vector>
#include "./Structures/Structures.hpp"
#include <openssl/params.h>
#include "./KuznechikContextAVX2/KuznechikContextAVX2.hpp"
#include "g_params.hpp"
#include "prov/num.h"

extern const OSSL_DISPATCH kuznechik2Functions[];
extern const OSSL_DISPATCH magma2Functions[];
extern const OSSL_DISPATCH kuznechik512Functions[];
extern const OSSL_DISPATCH magma512Functions[];
extern const OSSL_DISPATCH magma512RegFunctions[];

static const OSSL_ALGORITHM ghostCiphersTable[] = 
{
    { "magmaAVX2", "provider=gost", magma2Functions, NULL },
    { "kuznechikAVX2", "provider=gost", kuznechik2Functions, NULL},
    { "kuznechikAVX512", "provider=gost", kuznechik512Functions, NULL },
    { "magmaAVX512", "provider=gost", magma512Functions, NULL },
    { "magmaAVX512Reg", "provider=gost", magma512RegFunctions, NULL },
    { NULL, NULL, NULL, NULL }
};

/*
* Структура, описывающая контекст провайдера.
*/
struct provider_ctx_st
{
    OSSL_CORE_HANDLE *handle;
};

static void gostProvTeardown(void *providerCtx);
static const OSSL_ALGORITHM *gostProvOperation(void *providerCtx, int operationId, int *noCache);

/*
* OSSL_DISPATCH для провайдера.
*/
static const OSSL_DISPATCH providerFunctions[] = {
    { OSSL_FUNC_PROVIDER_TEARDOWN, (void (*)(void))gostProvTeardown },
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))gostProvOperation },
    { 0, NULL }
};

/*
* Освобождение ресурсов, связанных с контекстом провайдера.
*/
static void providerCtxFree(struct provider_ctx_st *ctx);

/*
* Создание нового контекста провайдера.
*/
static struct provider_ctx_st *providerCtxNew(const OSSL_CORE_HANDLE *handle, const OSSL_DISPATCH *in);

