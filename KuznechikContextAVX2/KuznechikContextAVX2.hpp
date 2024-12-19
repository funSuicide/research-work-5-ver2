#pragma once
#include "../KuznechikAVX2/KuznechikAVX2.hpp"
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/params.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <climits>
#include <cstring>
#include "../g_params.hpp"
#include "prov/num.h"

#define KEY_LENGTH_KUZ_AVX2 32      // Размер ключа для алгоритма "Кузнечик" в байтах.
#define BLOCK_SIZE_KUZ_AVX2 32      // Размер блока данных, используемого при шифровании и дешифровании алгоритмом "Кузнечик" для случая AVX2.
#define IV_LENGTH_KUZ_AVX2 8        // Размер вектора инициаилизации в байтах.

/*
 * @brief Структура, описывающая контекст провайдера gost.
*/
struct provider_ctx_st;

/*
 * @brief Структура, описывающая контекст алгоритма "Кузнечик" для случая AVX2.
*/
struct kuznechik2CtxSt
{
    provider_ctx_st *provCtx;   ///< Указатель на контекст провайдера.
    size_t keyl;    ///< Размер ключа шифрования.
    size_t ivl;     ///< Размер вектора инициализации.
    unsigned char *key;    ///< Буффер для хранения копии ключа в виде массива байт.                            
    unsigned char *iv;      ///< Буффео для хранения копии вектора инициализации в виде массива байт.
    uint64_t ivu;     ///< Вектор инициализации, представленный в виде uint64_t.           
    KuznechikAVX2 K;    ///< Экземпляр класса "Кузнечик" для случая AVX2.
    size_t sizeBlock;    ///< Размер блоков данных, используемых алгоритмом "Кузнечик" для случая AVX2.
    std::vector<byteVectorKuznechik> buffer2;    ///< Внутренний буффер контекста алгоритма, содержащий данные для операций.
    size_t partialBlockLen;     ///< Число, описывающее фактический размер последнего необработанного блкоа данных.
    size_t last;    ///< Число, описывающее количество обработанных данных (в байтах).
};

/*
 * Создание нового контекста алгоритма "Кузнечик" для случая AVX2.
 * @param provCtx указатель на контекст провайдера.
 * @return новый контекст алгоритма "Кузнечик" для случая AVX2.
*/
static void *kuznechik2NewCtx(void *provCtx);

/*
 * Освобождение ресурсов, связанных с контекстом.
 * @param provCtx указатель на контекст алгоритма "Кузнечик" для случая AVX2.
 * @return void
*/
static void kuznechik2FreeCtx(void *kuznechik2Ctx);

/*
 * Инициализация операции шифрования/дешифрования алгоритма "Кузнечик" для случая AVX2.
 * @param kuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param k указатель на массив байт, содержащих мастер-ключ,
 * @param keyl размер входящего мастер-ключа в байтах,
 * @param iv указатель на массив байт, содержащих знаение вектора инициализации,
 * @param ivlen размер входящего вектора инициализации,
 * @param params массив параметров для инициализации операции.
 * @return число, описывающее успешность инициализации.
*/
static int kuznechik2OperationInit(void *kuznechik2Ctx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[]);
    
/*
 * Операция алгоритма "Кузнечик" для случая AVX2.
 * @paramkuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param out указатель на массив байт, содержащих результат операций алгоритма,
 * @param outl указатель на размер записываемых данных,
 * @param outsz размер данных, который не должен превышать outl при записи,
 * @param in указатель на массив байт, содержащих входные данные,
 * @param inl размер входных данных.
 * @return число, описывающее успешность операции.
*/
static int kuznechik2Update(void *kuznechik2Ctx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl);

/*
 * Функция-финализатор алгоритма "Кузнечик" для случая AVX2.
 * @paramkuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param out указатель на массив байт, содержащих результат операций алгоритма, 
 * @param outsz размер данных, который не должен превышать outl при записи.
 * @return число, описывающее успешность операции.
*/
static int kuznechik2Final(void *kuznechik2ctx, unsigned char *out, size_t *outl, size_t outsize);

/*
 * Получение информации о параметрах алгоритма и её сохранение в params.
 * @param params массив параметров для сохранения.
 * @return число, описывающее успешность получения параметров.
*/
static int kuznechik2GetParams(OSSL_PARAM params[]);

/*
 * Получение нформации о параметрах алгоритма и её сохранение в params на стороне провайдера kuznechik2Ctx.
 * @param kuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param params - params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int kuznechik2GetCtxParams(void *kuznechik2Ctx, OSSL_PARAM params[]);

/*
 * Устанавливает параметры операции шифрования для контекста шифрования на стороне провайдера kuznechik2Ctx в params.
 * @param kuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param params params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int kuznechik2SetCtxParams(void *kuznechik2Ctx, const OSSL_PARAM params[]);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать kuznechik2GetCtxParams.
 * @param kuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *kuznechik2GetTableCtxParams(void *kuznechik2Ctx, void *provСtx);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать kuznechik2GetCtxParams.
 * @param kuznechik2Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX2,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *kuznechik2SetTableCtxParams(void *kuznechik2Ctx, void *provСtx);


static OSSL_FUNC_cipher_newctx_fn kuznechik2NewCtx; 
static OSSL_FUNC_cipher_freectx_fn kuznechik2FreeCtx; 
static OSSL_FUNC_cipher_encrypt_init_fn kuznechik2OperationInit; 
static OSSL_FUNC_cipher_decrypt_init_fn kuznechik2OperationInit; 
static OSSL_FUNC_cipher_update_fn kuznechik2Update; 
static OSSL_FUNC_cipher_final_fn kuznechik2Final; 
static OSSL_FUNC_cipher_get_params_fn kuznechik2GetParams;
static OSSL_FUNC_cipher_gettable_params_fn kuznechik2GetTableParams;
static OSSL_FUNC_cipher_set_ctx_params_fn kuznechik2SetCtxParams;
static OSSL_FUNC_cipher_get_ctx_params_fn kuznechik2GetCtxParams;
static OSSL_FUNC_cipher_settable_ctx_params_fn kuznechik2SetTableCtxParams;
static OSSL_FUNC_cipher_gettable_ctx_params_fn kuznechik2GetTableCtxParams;
