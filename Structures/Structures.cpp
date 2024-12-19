#include "Structures.hpp"

halfVectorKuznechik::halfVectorKuznechik(const uint64_t src) : halfVector(src) {}

byteVectorKuznechik::byteVectorKuznechik(const halfVectorKuznechik& lo, const halfVectorKuznechik& hi) : halfsData{lo, hi} {}

byteVectorKuznechik::byteVectorKuznechik(uint8_t* data)
{
	std::copy_n(data, 16, bytes);
}


byteVectorKuznechik::byteVectorKuznechik(uint8_t byte)
{
	for (size_t i = 0; i < 16; ++i)
	{
		this->bytes[i] = byte;
	}
}

key::key(uint8_t* data)
{
	std::copy_n(data, 32, bytes);
}

halfVectorMagma::halfVectorMagma(const uint32_t src) : vector{ src } {}

byteVectorMagma::byteVectorMagma(const halfVectorMagma& lo, const halfVectorMagma& hi) : halfsData{lo, hi} {}

byteVectorMagma::byteVectorMagma(uint16_t l0, uint16_t l1, uint16_t l2, uint16_t l3) : quartersData{l0, l1, l2, l3} {}
byteVectorMagma::byteVectorMagma(uint8_t* data)
{
	std::copy_n(data, 8, bytes);
}

byteVectorMagma::byteVectorMagma(uint8_t byte)
{
	for (size_t i = 0; i < 8; ++i)
	{
		this->bytes[i] = byte;
	}
}