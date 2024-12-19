#pragma once
#include "../KuznechikAVX512/KuznechikAVX512.hpp"
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/params.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <climits>
#include <cstring>
#include "../g_params.hpp"
#include "prov/num.h"

#define KEY_LENGTH_KUZ_AVX512 32      // Размер ключа для алгоритма "Кузнечик" в байтах.
#define BLOCK_SIZE_KUZ_AVX512 64      // Размер блока данных, используемого при шифровании и дешифровании алгоритмом "Кузнечик" для случая AVX512.
#define IV_LENGTH_KUZ_AVX512 8        // Размер вектора инициаилизации в байтах.

/*
 * @brief Структура, описывающая контекст провайдера gost.
*/
struct provider_ctx_st;

/*
 * @brief Структура, описывающая контекст алгоритма "Кузнечик" для случая AVX512.
*/
struct kuznechik512CtxSt
{
    provider_ctx_st *provCtx;   ///< Указатель на контекст провайдера.
    size_t keyl;    ///< Размер ключа шифрования.
    size_t ivl;     ///< Размер вектора инициализации.
    unsigned char *key;    ///< Буффер для хранения копии ключа в виде массива байт.                            
    unsigned char *iv;      ///< Буффео для хранения копии вектора инициализации в виде массива байт.
    uint64_t ivu;     ///< Вектор инициализации, представленный в виде uint64_t.           
    KuznechikAVX512 K;    ///< Экземпляр класса "Кузнечик" для случая AVX512.
    size_t sizeBlock;    ///< Размер блоков данных, используемых алгоритмом "Кузнечик" для случая AVX512.
    std::vector<byteVectorKuznechik> buffer2;    ///< Внутренний буффер контекста алгоритма, содержащий данные для операций.
    size_t partialBlockLen;     ///< Число, описывающее фактический размер последнего необработанного блкоа данных.
    size_t last;    ///< Число, описывающее количество обработанных данных (в байтах).
};

/*
 * Создание нового контекста алгоритма "Кузнечик" для случая AVX512.
 * @param provCtx указатель на контекст провайдера.
 * @return новый контекст алгоритма "Кузнечик" для случая AVX512.
*/
static void *kuznechik512NewCtx(void *provCtx);

/*
 * Освобождение ресурсов, связанных с контекстом.
 * @param provCtx указатель на контекст алгоритма "Кузнечик" для случая AVX512.
 * @return void
*/
static void kuznechik512FreeCtx(void *kuznechik512Ctx);

/*
 * Инициализация операции шифрования/дешифрования алгоритма "Кузнечик" для случая AVX512.
 * @param kuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param k указатель на массив байт, содержащих мастер-ключ,
 * @param keyl размер входящего мастер-ключа в байтах,
 * @param iv указатель на массив байт, содержащих знаение вектора инициализации,
 * @param ivlen размер входящего вектора инициализации,
 * @param params массив параметров для инициализации операции.
 * @return число, описывающее успешность инициализации.
*/
static int kuznechik512OperationInit(void *kuznechik512Ctx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[]);
    
/*
 * Операция алгоритма "Кузнечик" для случая AVX512.
 * @paramkuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param out указатель на массив байт, содержащих результат операций алгоритма,
 * @param outl указатель на размер записываемых данных,
 * @param outsz размер данных, который не должен превышать outl при записи,
 * @param in указатель на массив байт, содержащих входные данные,
 * @param inl размер входных данных.
 * @return число, описывающее успешность операции.
*/
static int kuznechik512Update(void *kuznechik512Ctx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl);

/*
 * Функция-финализатор алгоритма "Кузнечик" для случая AVX512.
 * @paramkuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param out указатель на массив байт, содержащих результат операций алгоритма, 
 * @param outsz размер данных, который не должен превышать outl при записи.
 * @return число, описывающее успешность операции.
*/
static int kuznechik512Final(void *kuznechik512ctx, unsigned char *out, size_t *outl, size_t outsize);

/*
 * Получение информации о параметрах алгоритма и её сохранение в params.
 * @param params массив параметров для сохранения.
 * @return число, описывающее успешность получения параметров.
*/
static int kuznechik512GetParams(OSSL_PARAM params[]);

/*
 * Получение нформации о параметрах алгоритма и её сохранение в params на стороне провайдера kuznechik512Ctx.
 * @param kuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param params - params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int kuznechik512GetCtxParams(void *kuznechik512Ctx, OSSL_PARAM params[]);

/*
 * Устанавливает параметры операции шифрования для контекста шифрования на стороне провайдера kuznechik512Ctx в params.
 * @param kuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param params params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int kuznechik512SetCtxParams(void *kuznechik512Ctx, const OSSL_PARAM params[]);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать kuznechik512GetCtxParams.
 * @param kuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *kuznechik512GetTableCtxParams(void *kuznechik512Ctx, void *provСtx);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать kuznechik512GetCtxParams.
 * @param kuznechik512Ctx указатель на контекст алгоритма "Кузнечик" для случая AVX512,
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *kuznechik512SetTableCtxParams(void *kuznechik512Ctx, void *provСtx);


static OSSL_FUNC_cipher_newctx_fn kuznechik512NewCtx; 
static OSSL_FUNC_cipher_freectx_fn kuznechik512FreeCtx; 
static OSSL_FUNC_cipher_encrypt_init_fn kuznechik512OperationInit; 
static OSSL_FUNC_cipher_decrypt_init_fn kuznechik512OperationInit; 
static OSSL_FUNC_cipher_update_fn kuznechik512Update; 
static OSSL_FUNC_cipher_final_fn kuznechik512Final; 
static OSSL_FUNC_cipher_get_params_fn kuznechik512GetParams;
static OSSL_FUNC_cipher_gettable_params_fn kuznechik512GetTableParams;
static OSSL_FUNC_cipher_set_ctx_params_fn kuznechik512SetCtxParams;
static OSSL_FUNC_cipher_get_ctx_params_fn kuznechik512GetCtxParams;
static OSSL_FUNC_cipher_settable_ctx_params_fn kuznechik512SetTableCtxParams;
static OSSL_FUNC_cipher_gettable_ctx_params_fn kuznechik512GetTableCtxParams;
