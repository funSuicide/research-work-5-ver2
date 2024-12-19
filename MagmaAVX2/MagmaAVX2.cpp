#include "MagmaAVX2.hpp"

static inline void expandKeys(const key& key, halfVectorMagma(&roundKeys)[8][8])
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7-i];
		}
	}
}

static inline __m256i getStartGammaBlocks(uint32_t iV)
{
	uint32_t tmp[8] = {};
	tmp[0] = iV;
	tmp[1] = 0x00;
	tmp[2] = iV + 0x01;
	tmp[3] = 0x00;
	tmp[4] = iV + 0x02;
	tmp[5] = 0x00;
	tmp[6] = iV + 0x03;
	tmp[7] = 0x00;
	return _mm256_loadu_si256((const __m256i*)tmp);
}

static inline __m256i invBytes(__m256i data)
{
	uint32_t mask[] = { 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
	__m256i mask2 = _mm256_loadu_si256((const __m256i*)mask);
	return _mm256_shuffle_epi8(data, mask2);
}


static inline __m256i tTransofrmAVX2(__m256i data)
{
	const int loMask = 0x0;
	const int hiMask = 0x1;

	__m256i divTmp11 = _mm256_shufflehi_epi16(data, 0xD8);
	__m256i divTmp12 = _mm256_shufflelo_epi16(divTmp11, 0xD8);

	__m256i divTmp21 = _mm256_shuffle_epi32(divTmp12, 0xD8);
	__m256i divTmp22 = _mm256_shuffle_epi32(divTmp12, 0x8D);

	__m128i divTmp31 = _mm256_extracti128_si256(divTmp21, hiMask);
	__m128i divTmp32 = _mm256_extracti128_si256(divTmp21, loMask);
	__m128i divTmp33 = _mm256_extracti128_si256(divTmp22, hiMask);
	__m128i divTmp34 = _mm256_extracti128_si256(divTmp22, loMask);

	__m128i hi = _mm_blend_epi32(divTmp31, divTmp34, 0x3);
	__m128i lo = _mm_blend_epi32(divTmp33, divTmp32, 0x3);

	__m256i expandedLo = _mm256_cvtepu16_epi32(lo);
	__m256i expandedHi = _mm256_cvtepu16_epi32(hi);

	__m256i resultLo = _mm256_i32gather_epi32((int const*)sTable2x65536[0], expandedLo, 2);
	__m256i resultHi = _mm256_i32gather_epi32((int const*)sTable2x65536[1], expandedHi, 2);

	__m256i recTmp11 = _mm256_shufflehi_epi16(resultHi, 0xB1);
	__m256i recTmp12 = _mm256_shufflelo_epi16(recTmp11, 0xB1);

	__m256i result = _mm256_blend_epi16(recTmp12, resultLo, 0x55);

	return result;
}

static inline __m256i cyclicShift11(__m256i data)
{
	__m256i resultShift = _mm256_slli_epi32(data, 11);
	__m256i resultShift2 = _mm256_srli_epi32(data, 21);
	return _mm256_xor_si256(resultShift, resultShift2);
}

static inline __m256i gTransformationAVX(const halfVectorMagma* roundKeyAddr, const __m256i data)
{
	__m256i vectorKey = _mm256_loadu_si256((const __m256i*)roundKeyAddr);
	__m256i result = _mm256_add_epi32(vectorKey, data);
	result = tTransofrmAVX2(result);
	result = cyclicShift11(result);
	result = invBytes(result);
	return result;
}

static inline void transformationGaVX(__m256i& loHalfs, __m256i& hiHalfs, const halfVectorMagma* roundKeyAddr) 
{
	__m256i gResult = gTransformationAVX(roundKeyAddr, loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

static inline void encryptEightBlocks(__m256i& loHalfs, __m256i& hiHalfs, const halfVectorMagma(&roundKeys)[8][8])
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - i]);
	}
	__m256i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

static inline void decryptEightBlocks(__m256i& loHalfs, __m256i& hiHalfs, const halfVectorMagma(&roundKeys)[8][8])
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i]);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - (i % 8)]);
	}
	__m256i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m256i tmp = _mm256_xor_si256(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

MagmaAVX2::MagmaAVX2()
{
	
}

MagmaAVX2::MagmaAVX2(const key& key) {
	expandKeys(key, this->roundKeys);
}

void MagmaAVX2::changeKey(const key& key)
{
	expandKeys(key, this->roundKeys);
}

void MagmaAVX2::processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x55; 
	const int loHalfsMask = 0xAA;
	for (size_t b = 0; b < src.size(); b += 8)
	{
		__m256i blocks1 = _mm256_loadu_si256((const __m256i*)(src.data() + b));
		__m256i blocks2 = _mm256_loadu_si256((const __m256i*)(src.data() + b + 4));


		__m256i blocks1Tmp = _mm256_shuffle_epi32(blocks1, blockMask);
		__m256i blocks2Tmp = _mm256_shuffle_epi32(blocks2, blockMask);

		__m256i loHalfs = _mm256_blend_epi32(blocks1, blocks2Tmp, loHalfsMask);
		__m256i hiHalfs = _mm256_blend_epi32(blocks2, blocks1Tmp, hiHalfsMask);

		if (en)
		{
			encryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);
		}
		else {
			decryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);
		}

		__m256i tmp = _mm256_shuffle_epi32(hiHalfs, blockMask);
		__m256i tmp2 = _mm256_shuffle_epi32(loHalfs, blockMask);

		blocks1 = _mm256_blend_epi32(loHalfs, tmp, loHalfsMask);
		blocks2 = _mm256_blend_epi32(tmp2, hiHalfs, loHalfsMask);

		_mm256_storeu_si256((__m256i*)(dest.data() + b), blocks1);
		_mm256_storeu_si256((__m256i*)(dest.data() + b + 4), blocks2);
	}
}

void MagmaAVX2::processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x55; 
	const int loHalfsMask = 0xAA;
	uint32_t diffGamma[8] = {0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00};
	__m256i diffGammaReg =  _mm256_loadu_si256((const __m256i*)diffGamma);

	__m256i gammaBlocks1 = getStartGammaBlocks(iV);
	__m256i gammaBlocks2 = _mm256_add_epi32(gammaBlocks1, diffGammaReg);

	for (size_t b = 0; b < src.size(); b += 8)
	{
		__m256i blocks1 = _mm256_loadu_si256((const __m256i*)(src.data() + b));
		__m256i blocks2 = _mm256_loadu_si256((const __m256i*)(src.data() + b + 4));

		__m256i blocks1Tmp = _mm256_shuffle_epi32(gammaBlocks1, blockMask);
		__m256i blocks2Tmp = _mm256_shuffle_epi32(gammaBlocks2, blockMask);

		__m256i loHalfs = _mm256_blend_epi32(gammaBlocks1, blocks2Tmp, loHalfsMask);
		__m256i hiHalfs = _mm256_blend_epi32(gammaBlocks2, blocks1Tmp, hiHalfsMask);

		encryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);

		__m256i tmp = _mm256_shuffle_epi32(hiHalfs, blockMask);
		__m256i tmp2 = _mm256_shuffle_epi32(loHalfs, blockMask);

		tmp = _mm256_blend_epi32(loHalfs, tmp, loHalfsMask);
		tmp2 = _mm256_blend_epi32(tmp2, hiHalfs, loHalfsMask);

		blocks1 = _mm256_xor_si256(blocks1, tmp);
		blocks2 = _mm256_xor_si256(blocks2, tmp2);

		_mm256_storeu_si256((__m256i*)(dest.data() + b), blocks1);
		_mm256_storeu_si256((__m256i*)(dest.data() + b + 4), blocks2);

		gammaBlocks1 = _mm256_add_epi32(gammaBlocks1, diffGammaReg);
		gammaBlocks2 = _mm256_add_epi32(gammaBlocks2, diffGammaReg);
	}
}