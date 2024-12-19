#pragma once
#include <cstdint>
#include <algorithm>

struct key {
    uint8_t bytes[32];
    explicit key(uint8_t* data);
    key() = default;
};

union halfVectorKuznechik {
    uint8_t bytes[sizeof(uint64_t)];
    uint64_t halfVector;
    halfVectorKuznechik() = default;
    halfVectorKuznechik(const uint64_t src);
};

union byteVectorKuznechik {
    struct halfs 
    {
        halfVectorKuznechik lo, hi;
    } halfsData;
    uint8_t bytes[sizeof(uint64_t) * 2];
    byteVectorKuznechik() = default;
    byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi);
    explicit byteVectorKuznechik(uint8_t* src);
    byteVectorKuznechik(uint8_t byte);
};

union halfVectorMagma {
    uint8_t bytes[sizeof(uint32_t)];
    uint32_t vector;
    halfVectorMagma() = default;
    halfVectorMagma(const uint32_t src);
};

union byteVectorMagma {
    struct halfs
    {
        halfVectorMagma lo, hi;
    } halfsData;
    struct quarters
    {
        uint16_t l0, l1, l2, l3;
    } quartersData;
    uint8_t bytes[8];
    uint64_t ull;
    byteVectorMagma() = default;
    byteVectorMagma(const halfVectorMagma& left, const halfVectorMagma& right); //+
    byteVectorMagma(uint16_t l0, uint16_t l1, uint16_t l2, uint16_t l3);
    explicit byteVectorMagma(uint8_t* data); // +
    byteVectorMagma(uint8_t);
};

enum Mode {
	ENCRYPT,
	DECRYPT,
};

enum Algorithm {
	KUZNECHIK_AVX2,
	KUZNECHIK_AVX512,
	MAGMA_AVX2,
	MAGMA_AVX512,
	MAGMA_AVX512_REG,
};