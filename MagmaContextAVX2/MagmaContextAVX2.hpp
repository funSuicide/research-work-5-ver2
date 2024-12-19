#pragma once
#include "../MagmaAVX2/MagmaAVX2.hpp"
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/params.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <climits>
#include <cstring>
#include "../g_params.hpp"
#include "prov/num.h"

#define KEY_LENGTH_MAG_AVX2 32      // Размер ключа для алгоритма "Магма" в байтах.
#define BLOCK_SIZE_MAG_AVX2 64      // Размер блока данных, используемого при шифровании и дешифровании алгоритмом "Магма" для случая AVX2.
#define IV_LENGTH_MAG_AVX2 4        // Изменить в будущем, пока что проблема с вектором иннициализации (см. вопрос 1).

/*
 * @brief Структура, описывающая контекст провайдера gost.
*/
struct provider_ctx_st;

/*
 * @brief Структура, описывающая контекст алгоритма "Магма" для случая AVX2.
*/
struct magma2CtxSt
{
    provider_ctx_st *provCtx;   ///< Указатель на контекст провайдера.
    size_t keyl;    ///< Размер ключа шифрования.
    size_t ivl;     ///< Размер вектора инициализации.
    unsigned char *key;    ///< Буффер для хранения копии ключа в виде массива байт.                            
    unsigned char *iv;      ///< Буффео для хранения копии вектора инициализации в виде массива байт.
    uint32_t ivu;     ///< Вектор инициализации, представленный в виде uint64_t.           
    MagmaAVX2 M;    ///< Экземпляр класса "Магма" для случая AVX2.
    size_t sizeBlock;    ///< Размер блоков данных, используемых алгоритмом "Магма" для случая AVX2.
    std::vector<byteVectorMagma> buffer2;    ///< Внутренний буффер контекста алгоритма, содержащий данные для операций.
    size_t partialBlockLen;     ///< Число, описывающее фактический размер последнего необработанного блкоа данных.
    size_t last;    ///< Число, описывающее количество обработанных данных (в байтах).
};

/*
 * Создание нового контекста алгоритма "Магма" для случая AVX2.
 * @param provCtx указатель на контекст провайдера.
 * @return новый контекст алгоритма "Магма" для случая AVX2.
*/
static void *magma2NewCtx(void *provCtx);

/*
 * Освобождение ресурсов, связанных с контекстом.
 * @param provCtx указатель на контекст алгоритма "Магма" для случая AVX2.
 * @return void
*/
static void magma2FreeCtx(void *magma2Ctx);

/*
 * Инициализация операции шифрования/дешифрования алгоритма "Магма" для случая AVX2.
 * @param magma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param k указатель на массив байт, содержащих мастер-ключ,
 * @param keyl размер входящего мастер-ключа в байтах,
 * @param iv указатель на массив байт, содержащих знаение вектора инициализации,
 * @param ivlen размер входящего вектора инициализации,
 * @param params массив параметров для инициализации операции.
 * @return число, описывающее успешность инициализации.
*/
static int magma2OperationInit(void *magma2Ctx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[]);
        
/*
 * Операция алгоритма "Магма" для случая AVX2.
 * @parammagma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param out указатель на массив байт, содержащих результат операций алгоритма,
 * @param outl указатель на размер записываемых данных,
 * @param outsz размер данных, который не должен превышать outl при записи,
 * @param in указатель на массив байт, содержащих входные данные,
 * @param inl размер входных данных.
 * @return число, описывающее успешность операции.
*/
static int magma2Update(void *magma2Ctx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl);

/*
 * Функция-финализатор алгоритма "Магма" для случая AVX2.
 * @parammagma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param out указатель на массив байт, содержащих результат операций алгоритма, 
 * @param outsz размер данных, который не должен превышать outl при записи.
 * @return число, описывающее успешность операции.
*/
static int magma2Final(void *magma2ctx, unsigned char *out, size_t *outl, size_t outsize);

/*
 * Получение информации о параметрах алгоритма и её сохранение в params.
 * @param params массив параметров для сохранения.
 * @return число, описывающее успешность получения параметров.
*/
static int magma2GetParams(OSSL_PARAM params[]);

/*
 * Получение нформации о параметрах алгоритма и её сохранение в params на стороне провайдера magma2Ctx.
 * @param magma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param params - params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int magma2GetCtxParams(void *magma2Ctx, OSSL_PARAM params[]);

/*
 * Устанавливает параметры операции шифрования для контекста шифрования на стороне провайдера magma2Ctx в params.
 * @param magma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param params params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int magma2SetCtxParams(void *magma2Ctx, const OSSL_PARAM params[]);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать magma2GetCtxParams.
 * @param magma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *magma2GetTableCtxParams(void *magma2Ctx, void *provСtx);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать magma2GetCtxParams.
 * @param magma2Ctx указатель на контекст алгоритма "Магма" для случая AVX2,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *magma2SetTableCtxParams(void *magma2Ctx, void *provСtx);


static OSSL_FUNC_cipher_newctx_fn magma2NewCtx; 
static OSSL_FUNC_cipher_freectx_fn magma2FreeCtx; 
static OSSL_FUNC_cipher_encrypt_init_fn magma2OperationInit; 
static OSSL_FUNC_cipher_decrypt_init_fn magma2OperationInit; 
static OSSL_FUNC_cipher_update_fn magma2Update; 
static OSSL_FUNC_cipher_final_fn magma2Final; 
static OSSL_FUNC_cipher_get_params_fn magma2GetParams;
static OSSL_FUNC_cipher_gettable_params_fn magma2GetTableParams;
static OSSL_FUNC_cipher_set_ctx_params_fn magma2SetCtxParams;
static OSSL_FUNC_cipher_get_ctx_params_fn magma2GetCtxParams;
static OSSL_FUNC_cipher_settable_ctx_params_fn magma2SetTableCtxParams;
static OSSL_FUNC_cipher_gettable_ctx_params_fn magma2GetTableCtxParams;
