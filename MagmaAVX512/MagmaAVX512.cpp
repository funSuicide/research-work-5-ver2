#include "MagmaAVX512.hpp"
#include <iostream>
#include "table4X256.hpp"
#include "table2X65536.hpp"

static inline __m512i invBytes(__m512i data)
{
	uint32_t mask[] = { 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
	__m512i mask2 = _mm512_loadu_epi32((const __m512i*)mask);
	return _mm512_shuffle_epi8(data, mask2);
}

static inline __m512i getStartGammaBlocksAVX512(uint32_t iV)
{
	uint32_t tmp[16] = {};
	tmp[0] = iV;
	tmp[1] = 0x00;
	tmp[2] = iV + 0x01;
	tmp[3] = 0x00;
	tmp[4] = iV + 0x02;
	tmp[5] = 0x00;
	tmp[6] = iV + 0x03;
	tmp[7] = 0x00;
    tmp[8] = iV + 0x04;
	tmp[9] = 0x00;
	tmp[10] = iV + 0x05;
	tmp[11] = 0x00;
	tmp[12] = iV + 0x06;
	tmp[13] = 0x00;
	tmp[14] = iV + 0x07;
	tmp[15] = 0x00;
	return _mm512_loadu_si512((const __m512i*)tmp);
}

static inline __m512i tTransofrmAVX512(__m512i data)
{
	const int loMask = 0x0;
	const int hiMask = 0x1;

	__m512i divTmp11 = _mm512_shufflehi_epi16(data, 0xD8);
	__m512i divTmp12 = _mm512_shufflelo_epi16(divTmp11, 0xD8);

	__m512i divTmp21 = _mm512_shuffle_epi32(divTmp12, (_MM_PERM_ENUM)0xD8);
	__m512i divTmp22 = _mm512_shuffle_epi32(divTmp12, (_MM_PERM_ENUM)0x8D);

	__m256i divTmp31 = _mm512_extracti64x4_epi64(divTmp21, hiMask);
	__m256i divTmp32 = _mm512_extracti64x4_epi64(divTmp21, loMask);
	__m256i divTmp33 = _mm512_extracti64x4_epi64(divTmp22, hiMask);
	__m256i divTmp34 = _mm512_extracti64x4_epi64(divTmp22, loMask);

	__m256i hi = _mm256_blend_epi32(divTmp31, divTmp34, 0x33);
	__m256i lo = _mm256_blend_epi32(divTmp33, divTmp32, 0x33);

	__m256i sLo = _mm256_shuffle_epi32(lo, 0x4E);
	__m256i ssLo = _mm256_permute4x64_epi64(sLo, 0x4E);
	__m256i sssLo = _mm256_blend_epi32(lo, ssLo, 0x3C);

	__m256i sHi = _mm256_shuffle_epi32(hi, 0x4E);
	__m256i ssHi = _mm256_permute4x64_epi64(sHi, 0x4E);
	__m256i sssHi = _mm256_blend_epi32(hi, ssHi, 0x3C);
	
	__m512i expandedLo = _mm512_cvtepu16_epi32(sssLo);
	__m512i expandedHi = _mm512_cvtepu16_epi32(sssHi);

	__m512i resultLo = _mm512_i32gather_epi32(expandedLo, (int const*)sTable2x65536[0], 2);
	__m512i resultHi = _mm512_i32gather_epi32(expandedHi, (int const*)sTable2x65536[1], 2);

	__m512i recTmp11 = _mm512_shufflehi_epi16(resultHi, 0xB1);
	__m512i recTmp12 = _mm512_shufflelo_epi16(recTmp11, 0xB1);

	__m512i result = _mm512_mask_blend_epi16(0x55555555, recTmp12, resultLo);

	return result;
}

static inline __m512i cyclicShift11(__m512i data)
{
	__m512i resultShift = _mm512_slli_epi32(data, 11);
	__m512i resultShift2 = _mm512_srli_epi32(data, 21);
	return _mm512_xor_epi32(resultShift, resultShift2);
}

static inline __m512i gTransformationAVX(const halfVectorMagma* roundKeyAddr, const __m512i data) 
{
	__m512i vectorKey = _mm512_loadu_epi32((const __m512i*)roundKeyAddr);
	__m512i result = _mm512_add_epi32(vectorKey, data);
	result = tTransofrmAVX512(result);
	result = cyclicShift11(result);
	result = invBytes(result);
	return result;
}

static inline void transformationGaVX(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma* roundKeyAddr) 
{
	__m512i gResult = gTransformationAVX(roundKeyAddr, loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

static inline void  encryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma (&roundKeys)[8][16]) 
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - i]);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}

static inline void decryptEightBlocks(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma (&roundKeys)[8][16]) 
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[i]);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGaVX(loHalfs, hiHalfs, roundKeys[7 - (i % 8)]);
	}
	__m512i gResults = gTransformationAVX(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytes(gResults), hiHalfs);
	hiHalfs = tmp;
}


void MagmaAVX512::processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x5555; 
	const int loHalfsMask = 0xAAAA;
	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_loadu_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_loadu_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocksTmp = _mm512_shuffle_epi32(blocks1, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, blocks1, blocksTmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, blocks2, blocksTmp);

		
		if (en)
		{
			encryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);
		}
		else {
			decryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);
		}

		__m512i tmp = _mm512_shuffle_epi32(hiHalfs, (_MM_PERM_ENUM)blockMask);
		__m512i tmp2 = _mm512_shuffle_epi32(loHalfs, (_MM_PERM_ENUM)blockMask);

		blocks1 = _mm512_mask_blend_epi32(loHalfsMask, loHalfs, tmp);
		blocks2 = _mm512_mask_blend_epi32(loHalfsMask, tmp2, hiHalfs);

		_mm512_storeu_epi32((__m512i*)(dest.data() + b), blocks1);
		_mm512_storeu_epi32((__m512i*)(dest.data() + b + 8), blocks2);
	}
}

void MagmaAVX512::processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x5555; 
	const int loHalfsMask = 0xAAAA;

	uint32_t diffGamma[16] = {0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00};
	__m512i diffGammaReg =  _mm512_loadu_si512((const __m512i*)diffGamma);

	__m512i gammaBlocks1 = getStartGammaBlocksAVX512(iV);
	__m512i gammaBlocks2 = _mm512_add_epi32(gammaBlocks1, diffGammaReg);

	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_loadu_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_loadu_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocksTmp = _mm512_shuffle_epi32(gammaBlocks1, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, gammaBlocks1, blocksTmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, gammaBlocks2, blocksTmp);

		
		encryptEightBlocks(loHalfs, hiHalfs, this->roundKeys);

		__m512i tmp = _mm512_shuffle_epi32(hiHalfs, (_MM_PERM_ENUM)blockMask);
		__m512i tmp2 = _mm512_shuffle_epi32(loHalfs, (_MM_PERM_ENUM)blockMask);

		tmp = _mm512_mask_blend_epi32(loHalfsMask, loHalfs, tmp);
		tmp2 = _mm512_mask_blend_epi32(loHalfsMask, tmp2, hiHalfs);

        blocks1 = _mm512_xor_si512(blocks1, tmp);
		blocks2 = _mm512_xor_si512(blocks2, tmp2);

		_mm512_storeu_epi32((__m512i*)(dest.data() + b), blocks1);
		_mm512_storeu_epi32((__m512i*)(dest.data() + b + 8), blocks2);

        gammaBlocks1 = _mm512_add_epi32(gammaBlocks1, diffGammaReg);
		gammaBlocks2 = _mm512_add_epi32(gammaBlocks2, diffGammaReg);
	}
}

MagmaAVX512::MagmaAVX512(const key& key) {
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			this->roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7 - i];
		}
	}
}

void MagmaAVX512::changeKey(const key& key)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			this->roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7 - i];
		}
	}
}