#pragma once
#include <stdint.h>
#include <inttypes.h>
#include <immintrin.h>
#include <span>
#include "../Structures/Structures.hpp"
#include "../tables/table4X256.hpp"
#include "../tables/table2X65536.hpp"

class MagmaAVX2 {
private:
	halfVectorMagma roundKeys[8][8];
public:
	MagmaAVX2();
	MagmaAVX2(const key& key);
	void processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en)  const;
	void processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const;
	void changeKey(const key& key);
	halfVectorMagma getKey(int a, int b) 
    {
        return roundKeys[a][b];
    }
};
