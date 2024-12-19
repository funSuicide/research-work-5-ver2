#pragma once
#include "../MagmaAVX512Reg/MagmaAVX512Reg.hpp"
#include <openssl/core.h>
#include <openssl/core_dispatch.h>
#include <openssl/params.h>
#include <openssl/opensslconf.h>
#include <openssl/core_names.h>
#include <climits>
#include <cstring>
#include "../g_params.hpp"
#include "prov/num.h"

#define KEY_LENGTH_MAG_AVX512Reg 32      // Размер ключа для алгоритма "Магма" в байтах.
#define BLOCK_SIZE_MAG_AVX512Reg 128      // Размер блока данных, используемого при шифровании и дешифровании алгоритмом "Магма" для случая AVX512 (таблица замен в регистрах).
#define IV_LENGTH_MAG_AVX512Reg 4        // Изменить в будущем, пока что проблема с вектором иннициализации (см. вопрос 1).

/*
 * @brief Структура, описывающая контекст провайдера gost.
*/
struct provider_ctx_st;

/*
 * @brief Структура, описывающая контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
*/
struct magma512RegCtxSt
{
    provider_ctx_st *provCtx;   ///< Указатель на контекст провайдера.
    size_t keyl;    ///< Размер ключа шифрования.
    size_t ivl;     ///< Размер вектора инициализации.
    unsigned char *key;    ///< Буффер для хранения копии ключа в виде массива байт.                            
    unsigned char *iv;      ///< Буффео для хранения копии вектора инициализации в виде массива байт.
    uint32_t ivu;     ///< Вектор инициализации, представленный в виде uint64_t.           
    MagmaAVX512Reg M;    ///< Экземпляр класса "Магма" для случая AVX512 (таблица замен в регистрах).
    size_t sizeBlock;    ///< Размер блоков данных, используемых алгоритмом "Магма" для случая AVX512 (таблица замен в регистрах).
    std::vector<byteVectorMagma> buffer2;    ///< Внутренний буффер контекста алгоритма, содержащий данные для операций.
    size_t partialBlockLen;     ///< Число, описывающее фактический размер последнего необработанного блкоа данных.
    size_t last;    ///< Число, описывающее количество обработанных данных (в байтах).
};

/*
 * Создание нового контекста алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
 * @param provCtx указатель на контекст провайдера.
 * @return новый контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
*/
static void *magma512RegNewCtx(void *provCtx);

/*
 * Освобождение ресурсов, связанных с контекстом.
 * @param provCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
 * @return void
*/
static void magma512RegFreeCtx(void *magma512RegCtx);

/*
 * Инициализация операции шифрования/дешифрования алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
 * @param magma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param k указатель на массив байт, содержащих мастер-ключ,
 * @param keyl размер входящего мастер-ключа в байтах,
 * @param iv указатель на массив байт, содержащих знаение вектора инициализации,
 * @param ivlen размер входящего вектора инициализации,
 * @param params массив параметров для инициализации операции.
 * @return число, описывающее успешность инициализации.
*/
static int magma512RegOperationInit(void *magma512RegCtx, const unsigned char *k, size_t keyl, const unsigned char *iv, size_t ivlen, const OSSL_PARAM params[]);
        
/*
 * Операция алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
 * @parammagma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param out указатель на массив байт, содержащих результат операций алгоритма,
 * @param outl указатель на размер записываемых данных,
 * @param outsz размер данных, который не должен превышать outl при записи,
 * @param in указатель на массив байт, содержащих входные данные,
 * @param inl размер входных данных.
 * @return число, описывающее успешность операции.
*/
static int magma512RegUpdate(void *magma512RegCtx, unsigned char *out, size_t *outl, size_t outsz, const unsigned char *in, size_t inl);

/*
 * Функция-финализатор алгоритма "Магма" для случая AVX512 (таблица замен в регистрах).
 * @parammagma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param out указатель на массив байт, содержащих результат операций алгоритма, 
 * @param outsz размер данных, который не должен превышать outl при записи.
 * @return число, описывающее успешность операции.
*/
static int magma512RegFinal(void *magma512Regctx, unsigned char *out, size_t *outl, size_t outsize);

/*
 * Получение информации о параметрах алгоритма и её сохранение в params.
 * @param params массив параметров для сохранения.
 * @return число, описывающее успешность получения параметров.
*/
static int magma512RegGetParams(OSSL_PARAM params[]);

/*
 * Получение нформации о параметрах алгоритма и её сохранение в params на стороне провайдера magma512RegCtx.
 * @param magma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param params - params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int magma512RegGetCtxParams(void *magma512RegCtx, OSSL_PARAM params[]);

/*
 * Устанавливает параметры операции шифрования для контекста шифрования на стороне провайдера magma512RegCtx в params.
 * @param magma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param params params массив параметров.
 * @return число, описывающее успешность установки параметров.
*/
static int magma512RegSetCtxParams(void *magma512RegCtx, const OSSL_PARAM params[]);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать magma512RegGetCtxParams.
 * @param magma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *magma512RegGetTableCtxParams(void *magma512RegCtx, void *provСtx);

/*
 * Получение массива констант OSSL_PARAM в качестве дескрипторов параметров, который может обрабатывать magma512RegGetCtxParams.
 * @param magma512RegCtx указатель на контекст алгоритма "Магма" для случая AVX512 (таблица замен в регистрах),
 * @param provCtx указатель на контекст провайдера,
 * @return массив OSSL_PARAM.
*/
static const OSSL_PARAM *magma512RegSetTableCtxParams(void *magma512RegCtx, void *provСtx);


static OSSL_FUNC_cipher_newctx_fn magma512RegNewCtx; 
static OSSL_FUNC_cipher_freectx_fn magma512RegFreeCtx; 
static OSSL_FUNC_cipher_encrypt_init_fn magma512RegOperationInit; 
static OSSL_FUNC_cipher_decrypt_init_fn magma512RegOperationInit; 
static OSSL_FUNC_cipher_update_fn magma512RegUpdate; 
static OSSL_FUNC_cipher_final_fn magma512RegFinal; 
static OSSL_FUNC_cipher_get_params_fn magma512RegGetParams;
static OSSL_FUNC_cipher_gettable_params_fn magma512RegGetTableParams;
static OSSL_FUNC_cipher_set_ctx_params_fn magma512RegSetCtxParams;
static OSSL_FUNC_cipher_get_ctx_params_fn magma512RegGetCtxParams;
static OSSL_FUNC_cipher_settable_ctx_params_fn magma512RegSetTableCtxParams;
static OSSL_FUNC_cipher_gettable_ctx_params_fn magma512RegGetTableCtxParams;
