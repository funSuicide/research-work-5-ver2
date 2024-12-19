#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <immintrin.h>
#include <span>
#include "../Structures/Structures.hpp"

class KuznechikAVX2 
{
private:
    byteVectorKuznechik roundKeysKuznechik[10][2];
public:
    KuznechikAVX2() = default;
    KuznechikAVX2(const key& key);
    void processData(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en)  const;
    void processDataGamma(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, uint64_t iV) const;
    void changeKey(const key& key);
    byteVectorKuznechik getKey(int a, int b) 
    {
        return roundKeysKuznechik[a][b];
    }
};