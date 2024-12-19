#include "MagmaAVX512Reg.hpp"
#include <iostream>

register __m512i r1 asm ("zmm16");
register __m512i r2 asm ("zmm17");
register __m512i r3 asm ("zmm18");
register __m512i r4 asm ("zmm19");
register __m512i r5 asm ("zmm20");
register __m512i r6 asm ("zmm21");
register __m512i r7 asm ("zmm22");
register __m512i r8 asm ("zmm23");
register __m512i r9 asm ("zmm24");
register __m512i r10 asm ("zmm25");
register __m512i r11 asm ("zmm26");
register __m512i r12 asm ("zmm27");
register __m512i r13 asm ("zmm28");
register __m512i r14 asm ("zmm29");
register __m512i r15 asm ("zmm30");
register __m512i r16 asm ("zmm31");

static inline __m512i invBytesAVX512Reg(__m512i data)
{
	uint32_t mask[] = { 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F, 0x0010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };
	__m512i mask2 = _mm512_loadu_epi32((const __m512i*)mask);
	return _mm512_shuffle_epi8(data, mask2);
}

static inline __m512i getStartGammaBlocksAVX512Reg(uint32_t iV)
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

static inline __m512i tTransformInRegistersAVX512(const __m512i data) 
{
	__m512i result = _mm512_setzero_si512();

	const int maskByte1 = 0xFF000000;
	const int maskByte2 = 0x00FF0000;
	const int maskByte3 = 0x0000FF00;
	const int maskByte4 = 0x000000FF;

	const uint64_t tmpForMask1 = 0x1111111111111111;
	const uint64_t tmpForMask2 = 0x2222222222222222;
	const uint64_t tmpForMask3 = 0x4444444444444444;
	const uint64_t tmpForMask4 = 0x8888888888888888;

	__m512i tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte4), data);

	__m512i currentIndexes = _mm512_set1_epi8(32);
	__m512i tmpResult = _mm512_setzero_si512();

	__mmask64 currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	__mmask64 mainMask = currentMask;

	__m512i tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	__m512i tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	
	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	__mmask64 tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r1);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r2);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r3);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r4);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte4), tmpResult);
	result = _mm512_xor_epi32(result, tmp);


	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte3), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	
	tmp = tmp;

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r5);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r5);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r6);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r6);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r7);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r7);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r8);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r8);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte3), tmpResult);
	result = _mm512_xor_epi32(result, tmp);


	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte2), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	tmp = tmp;

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r9);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r9);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r10);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r10);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r11);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r11);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r12);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r12);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte2), tmpResult);
	result = _mm512_xor_epi32(result, tmp);


	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte1), data);

	currentIndexes = _mm512_set1_epi8(32);
	tmpResult = _mm512_setzero_si512();

	currentMask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	mainMask = currentMask;

	tmp = tmp;

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r13);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((1 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(0));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r13);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((2 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r14);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((3 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(64));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r14);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8(5 * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r15);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((5 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(128));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r15);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((6 + 1) * 32);
	tmpmask = _mm512_cmplt_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r16);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);


	currentIndexes = _mm512_set1_epi8((7 + 1) * 32 - 1);
	tmpmask = _mm512_cmple_epu8_mask(tmp, currentIndexes);
	currentMask = _kand_mask64(_knot_mask64(mainMask), tmpmask);

	mainMask = _kxor_mask64(currentMask, mainMask);

	tmpD = _mm512_sub_epi8(tmp, _mm512_set1_epi8(192));
	tmp2 = _mm512_maskz_permutexvar_epi8(currentMask, tmpD, r16);
	tmpResult = _mm512_xor_epi32(tmpResult, tmp2);

	tmp = _mm512_and_epi32(_mm512_set1_epi32(maskByte1), tmpResult);
	result = _mm512_xor_epi32(result, tmp);
	return result;
}

static inline __m512i cyclicShift11AVX512Reg(__m512i data)
{
	__m512i resultShift = _mm512_slli_epi32(data, 11);
	__m512i resultShift2 = _mm512_srli_epi32(data, 21);
	return _mm512_xor_epi32(resultShift, resultShift2);
}

static inline __m512i gTransformationAVX512Reg(const halfVectorMagma* roundKeyAddr, const __m512i data) 
{
	__m512i vectorKey = _mm512_loadu_epi32((const __m512i*)roundKeyAddr);
	__m512i result = _mm512_add_epi32(vectorKey, data);
	result = tTransformInRegistersAVX512(result);
	result = cyclicShift11AVX512Reg(result);
	result = invBytesAVX512Reg(result);
	return result;
}

static inline void transformationGAVX512Reg(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma* roundKeyAddr) 
{
	__m512i gResult = gTransformationAVX512Reg(roundKeyAddr, loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytesAVX512Reg(gResult), hiHalfs);
	hiHalfs = loHalfs;
	loHalfs = tmp;
}

static inline void  encryptEightBlocksGAVX512Reg(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma (&roundKeys)[8][16]) 
{
	for (size_t i = 0; i < 24; i++)
	{
		transformationGAVX512Reg(loHalfs, hiHalfs, roundKeys[i % 8]);
	}
	for (size_t i = 0; i < 7; i++)
	{
		transformationGAVX512Reg(loHalfs, hiHalfs, roundKeys[7 - i]);
	}
	__m512i gResults = gTransformationAVX512Reg(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytesAVX512Reg(gResults), hiHalfs);
	hiHalfs = tmp;
}

static inline void decryptEightBlocksGAVX512Reg(__m512i& loHalfs, __m512i& hiHalfs, const halfVectorMagma (&roundKeys)[8][16]) 
{
	for (size_t i = 0; i < 8; i++)
	{
		transformationGAVX512Reg(loHalfs, hiHalfs, roundKeys[i]);
	}
	for (size_t i = 0; i < 23; i++)
	{
		transformationGAVX512Reg(loHalfs, hiHalfs, roundKeys[7 - (i % 8)]);
	}
	__m512i gResults = gTransformationAVX512Reg(roundKeys[0], loHalfs);
	__m512i tmp = _mm512_xor_epi32(invBytesAVX512Reg(gResults), hiHalfs);
	hiHalfs = tmp;
}


void MagmaAVX512Reg::processData(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, bool en) const
{
	const int blockMask = 0xB1;
	const int hiHalfsMask = 0x5555; 
	const int loHalfsMask = 0xAAAA;

	r1 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]));
	r2 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+1);
	r3 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+2);
	r4 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+3);
	r5 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]));
	r6 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+1);
	r7 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+2);
	r8 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+3);
	r9 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]));
	r10 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+1);
	r11 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+2);
	r12 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+3);
	r13 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]));
	r14 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+1);
	r15 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+2);
	r16 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+3);

	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_loadu_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_loadu_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocksTmp = _mm512_shuffle_epi32(blocks1, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, blocks1, blocksTmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, blocks2, blocksTmp);

		
		if (en)
		{
			encryptEightBlocksGAVX512Reg(loHalfs, hiHalfs, this->roundKeys);
		}
		else {
			decryptEightBlocksGAVX512Reg(loHalfs, hiHalfs, this->roundKeys);
		}

		__m512i tmp = _mm512_shuffle_epi32(hiHalfs, (_MM_PERM_ENUM)blockMask);
		__m512i tmp2 = _mm512_shuffle_epi32(loHalfs, (_MM_PERM_ENUM)blockMask);

		blocks1 = _mm512_mask_blend_epi32(loHalfsMask, loHalfs, tmp);
		blocks2 = _mm512_mask_blend_epi32(loHalfsMask, tmp2, hiHalfs);

		_mm512_storeu_epi32((__m512i*)(dest.data() + b), blocks1);
		_mm512_storeu_epi32((__m512i*)(dest.data() + b + 8), blocks2);
	}
}

void MagmaAVX512Reg::processDataGamma(std::span<const byteVectorMagma> src, std::span<byteVectorMagma> dest, uint64_t iV) const
{
    const int blockMask = 0xB1;
	const int hiHalfsMask = 0x5555; 
	const int loHalfsMask = 0xAAAA;

	r1 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0]));
	r2 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+1);
	r3 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+2);
	r4 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[0])+3);
	r5 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1]));
	r6 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+1);
	r7 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+2);
	r8 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[1])+3);
	r9 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2]));
	r10 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+1);
	r11 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+2);
	r12 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[2])+3);
	r13 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3]));
	r14 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+1);
	r15 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+2);
	r16 =  _mm512_loadu_epi32((const __m512i*)(sTable4x256[3])+3);

    uint32_t diffGamma[16] = {0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00};
	__m512i diffGammaReg =  _mm512_loadu_si512((const __m512i*)diffGamma);

	__m512i gammaBlocks1 = getStartGammaBlocksAVX512Reg(iV);
	__m512i gammaBlocks2 = _mm512_add_epi32(gammaBlocks1, diffGammaReg);

	for (size_t b = 0; b < src.size(); b += 16)
	{
		__m512i blocks1 = _mm512_loadu_epi32((const __m512i*)(src.data() + b));
		__m512i blocks2 = _mm512_loadu_epi32((const __m512i*)(src.data() + b + 8));

		__m512i blocksTmp = _mm512_shuffle_epi32(gammaBlocks1, (_MM_PERM_ENUM)blockMask);

		__m512i loHalfs = _mm512_mask_blend_epi32(loHalfsMask, gammaBlocks1, blocksTmp);
		__m512i hiHalfs = _mm512_mask_blend_epi32(hiHalfsMask, gammaBlocks2, blocksTmp);

		encryptEightBlocksGAVX512Reg(loHalfs, hiHalfs, this->roundKeys);

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

MagmaAVX512Reg::MagmaAVX512Reg(const key& key) {
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			this->roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7 - i];
		}
	}
}

void MagmaAVX512Reg::changeKey(const key& key)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			this->roundKeys[i][j].vector = reinterpret_cast<const uint32_t*>(key.bytes)[7 - i];
		}
	}
}