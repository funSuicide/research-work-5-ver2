#include "KuznechikAVX2.hpp"
#include <array>
#include <algorithm>
#include <iostream>

static constexpr uint8_t sTable[256] = {
	0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16,
	0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
	0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA,
	0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
	0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21,
	0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
	0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0,
	0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
	0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB,
	0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
	0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12,
	0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
	0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7,
	0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
	0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E,
	0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
	0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9,
	0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,
	0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC,
	0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,
	0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44,
	0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,
	0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F,
	0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,
	0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7,
	0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,
	0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE,
	0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,
	0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B,
	0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,
	0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0,
	0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6 };

static constexpr uint8_t lVector[16] = { 1, 148, 32, 133, 16, 194, 192, 1, 251, 1, 192, 194, 16, 133, 32, 148 };

uint8_t multiplicationGalua(uint8_t first, uint8_t second) {
	uint8_t result = 0;
	uint8_t hiBit;
	for (int i = 0; i < 8;i++) {
		if (second & 1) {
			result ^= first;
		}
		hiBit = first & 0x80;
		first <<= 1;
		if (hiBit) {
			first ^= 0xc3;
		}
		second >>= 1;
	}
	return result;
}

byteVectorKuznechik xOR(const byteVectorKuznechik src1, const byteVectorKuznechik src2) {
	halfVectorKuznechik lo = src1.halfsData.lo.halfVector ^ src2.halfsData.lo.halfVector;
	halfVectorKuznechik hi = src1.halfsData.hi.halfVector ^ src2.halfsData.hi.halfVector;
	return byteVectorKuznechik(lo, hi);
}

byteVectorKuznechik transformationS(const byteVectorKuznechik src) {
	byteVectorKuznechik tmp{};
	for (size_t i = 0; i < 16; i++) {
		tmp.bytes[i] = sTable[src.bytes[i]];
	}
	return tmp;
}


byteVectorKuznechik transformationR(const byteVectorKuznechik src) {
	uint8_t a_15 = 0;
	static byteVectorKuznechik internal = { 0, 0 };
	for (int i = 15; i >= 0; i--) {
		if (i == 0)
		{
			internal.bytes[15] = src.bytes[i];
		}
		else
		{
			internal.bytes[i - 1] = src.bytes[i];
		}
		a_15 ^= multiplicationGalua(src.bytes[i], lVector[i]);
	}
	internal.bytes[15] = a_15;
	return internal;
}


byteVectorKuznechik transformaionL(const byteVectorKuznechik& inData) {
	static byteVectorKuznechik tmp;
	std::copy_n(inData.bytes, 16, tmp.bytes);
	for (int i = 0; i < 16; i++) {
		tmp = transformationR(tmp);
	}
	return tmp;
}

byteVectorKuznechik transformationF(const byteVectorKuznechik src, const byteVectorKuznechik cons) {
	byteVectorKuznechik tmp = {0, 0};
	tmp = xOR(src, cons);
	tmp = transformationS(tmp);
	byteVectorKuznechik d = {0, 0};
	d = transformaionL(tmp);
	return d;
}


std::array <byteVectorKuznechik, 32> getConstTableKuz() {
	std::array <byteVectorKuznechik, 32> constTable;
	byteVectorKuznechik numberIter = { halfVectorKuznechik(0), halfVectorKuznechik(0)};
	numberIter.bytes[0] += 0x01;
	for (int i = 0; i < 32; i++) {
		byteVectorKuznechik result = { 0, 0 };
		result = transformaionL(numberIter);
		constTable[i] = result;
		numberIter.bytes[0] += 0x01;
	}
	return constTable;
}


std::array<std::array<byteVectorKuznechik, 256>, 16> getStartTable() {
	std::array<std::array<byteVectorKuznechik, 256>, 16> startByteT;
	for (int j = 0; j < 16; ++j) {
		byteVectorKuznechik tmp{};
		for (int i = 0; i < 256; i++) {

			byteVectorKuznechik c = transformationS(tmp);

			for (size_t q = 0; q < 16; ++q)
			{
				if (q != j) c.bytes[q] = 0;
			}
			
			byteVectorKuznechik d = transformaionL(c);

			startByteT[j][i] = d;
			tmp.bytes[j] += 0x01;
		}
	}
	return startByteT;
}

std::array<std::array<byteVectorKuznechik, 256>, 16> startByteT = getStartTable();

std::array <byteVectorKuznechik, 32> constTable = getConstTableKuz();

void getRoundKeys(const key& mainKey, byteVectorKuznechik(&roundKeysKuznechik)[10][2]) {
	uint8_t lo[16];
	uint8_t hi[16];
	size_t numberKey = 0;
	std::copy(mainKey.bytes, mainKey.bytes + 16, lo);
	std::copy(mainKey.bytes + 16, mainKey.bytes + 32, hi);
	byteVectorKuznechik leftPart(lo);
	byteVectorKuznechik rightPart(hi);
	roundKeysKuznechik[0][0] = rightPart;
	roundKeysKuznechik[0][1] = rightPart;
	roundKeysKuznechik[1][0] = leftPart;
	roundKeysKuznechik[1][1] = leftPart;
	numberKey += 2;
	for (size_t i = 1; i < 5; i++) {
		int iter = 0;
		for (size_t j = 0; j < 8; j++) {
			byteVectorKuznechik tmp2 = leftPart;
			leftPart = rightPart;
			byteVectorKuznechik tmp = transformationF(rightPart, constTable[(8 * (i-1) + j)]);
			rightPart = xOR(tmp, tmp2);
			iter++;
		}
		roundKeysKuznechik[numberKey][0] = rightPart;
		roundKeysKuznechik[numberKey][1] = rightPart;
		numberKey++;
		roundKeysKuznechik[numberKey][0] = leftPart;
		roundKeysKuznechik[numberKey][1] = leftPart;
		numberKey++;
	}
}

__m256i encryptBlockAVX2(__m256i blocks, const byteVectorKuznechik(&roundKeysKuznechik)[10][2]) {
	__m256i result = blocks;
	byteVectorKuznechik t[2];
	for (size_t i = 0; i < 9; ++i) {
		__m256i keys = _mm256_loadu_si256((const __m256i*)roundKeysKuznechik[i]);
		
		result = _mm256_xor_si256(result, keys);
		_mm256_storeu_si256((__m256i*)t, result);

		
		__m256i tmp = _mm256_setzero_si256();
		for (size_t j = 0; j < 16; j++) {
			__m128i tmp1 = _mm_loadu_si128((const __m128i*) & startByteT[j][t[0].bytes[j]]);
			__m128i tmp2 = _mm_loadu_si128((const __m128i*) & startByteT[j][t[1].bytes[j]]);

			__m256i valuesAVX = _mm256_insertf128_si256(_mm256_castsi128_si256(tmp1), tmp2, 0x1);
			tmp = _mm256_xor_si256(tmp, valuesAVX);
		}

		result = tmp;
	}
	__m256i lastKeys = _mm256_loadu_si256((const __m256i*)roundKeysKuznechik[9]);
	result = _mm256_xor_si256(result, lastKeys);
	return result;
}

void KuznechikAVX2::processData(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, bool en) const
{
	for (size_t b = 0; b < src.size(); b += 2)
	{
		__m256i blocks = _mm256_loadu_si256((const __m256i*)(src.data() + b));

		__m256i result = _mm256_setzero_si256();

		if (en)
		{
			result = encryptBlockAVX2(blocks, this->roundKeysKuznechik);
		}

		_mm256_storeu_si256((__m256i*)(dest.data() + b), result);
	}
}

static inline __m256i getStartGammaBlocksKuznechik(uint64_t iV)
{
	uint64_t tmp[4] = {};
	tmp[0] = iV;
	tmp[1] = 0x00;
	tmp[3] = iV + 0x01;
	tmp[4] = 0x00;
	return _mm256_loadu_si256((const __m256i*)tmp);
}

 void KuznechikAVX2::processDataGamma(std::span<const byteVectorKuznechik> src, std::span<byteVectorKuznechik> dest, uint64_t iV) const
 {
	uint64_t diffGamma[4] = {0x02, 0x00, 0x02, 0x00};
	__m256i diffGammaReg =  _mm256_loadu_si256((const __m256i*)diffGamma);
	__m256i gammalocks = getStartGammaBlocksKuznechik(iV);

	for (size_t b = 0; b < src.size(); b += 2)
	{
		__m256i blocks = _mm256_loadu_si256((const __m256i*)(src.data() + b));

		__m256i result = _mm256_setzero_si256();

		result = encryptBlockAVX2(gammalocks, this->roundKeysKuznechik);

		result = _mm256_xor_si256(result, blocks);

		_mm256_storeu_si256((__m256i*)(dest.data() + b), result);

		gammalocks = _mm256_add_epi64(gammalocks, diffGammaReg);
	}
 }


KuznechikAVX2::KuznechikAVX2(const key& mainKey) {
	getRoundKeys(mainKey, this->roundKeysKuznechik);
}

void KuznechikAVX2::changeKey(const key& key)
{
	getRoundKeys(key, this->roundKeysKuznechik);
}