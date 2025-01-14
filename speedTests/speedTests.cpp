#include <iostream>
#include <vector>
#include <stdint.h>
#include <string>
#include <chrono>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "../CtxFactory/CtxFactory.hpp"

#define GIGABYTE 1024*1024*1024

using duration_t = std::chrono::duration<float>;

void generateRandomByteArray(std::vector<unsigned char>& byteArray, size_t size) {
    std::srand(static_cast<unsigned int>(std::time(0)));
    for (size_t i = 0; i < size; ++i)
    {
        byteArray[i] = static_cast<unsigned char>(std::rand() % 256);
    }
}

void testSpeed(char* prov, char* alg)
{
	std::cout << "Запуск теста скорости..." << std::endl;
	std::vector<float> times2;

	uint8_t key[32] = "asdafasdasdasfdasdasdakfksakfsa";
    uint8_t iv[8] = "bbbbeee";

	for (int j = 0; j < 5; ++j) {
        std::cout << "Шаг " << j << std::endl;
        std::vector<unsigned char> byteArray(GIGABYTE);
        CtxFactory F = CtxFactory(prov, alg);
        OsslCtx c = F.next(key, iv);

		auto begin = std::chrono::steady_clock::now();
		// шифрование
        char buffer[4096];
        
        for (size_t i = 0; i < GIGABYTE / 1024 - 1; ++i)
        {
            c.encrypt(byteArray.data() + 1024 * i, (unsigned char*)buffer, 1024);
        }
        c.final_encrypt((unsigned char*)buffer);

		auto end = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<duration_t>(end - begin);
		times2.push_back(time.count());
		std::cout << "Обработано: " << j+1 << " ГБ" << ": " << time.count() << std::endl;
	}

	double meanM2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
	std::cout << meanM2 << std::endl;
	std::cout << "Среднее значение скорости алгоритма: " << 1 / meanM2 << "ГБ/с" << std::endl;

	std::cout << "----------------------------------------------" << std::endl;
}

int main()
{       
        
        /*
		std::cout << "Тестирование алгоритма Магма AVX-2" << std::endl;
    	testSpeed("gost", "magmaAVX2");
    	std::cout << "Тестирование алгоритма Кузнечик AVX-2" << std::endl;
    	testSpeed("gost", "kuznechikAVX2");
    	std::cout << "Тестирование алгоритма Кузнечие AVX-512" << std::endl;
    	testSpeed("gost", "kuznechikAVX512");
		std::cout << "Тестирование алгоритма Магма AVX-512 (таблица в памяти)" << std::endl;
    	testSpeed("gost", "magmaAVX512");
		std::cout << "Тестирование алгоритма Магма AVX-512 (таблица в регистрах)" << std::endl;
    	testSpeed("gost", "magmaAVX512Reg");

        // other

        std::cout << "Тестирование алгоритма Магма (сторонний провайдер)" << std::endl;
    	testSpeed("testgost", "magma-ctr");
        std::cout << "Тестирование алгоритма Кузнечик (сторонний провайдер)" << std::endl;
    	testSpeed("testgost", "kuznyechik-ctr");
        */

        std::cout << "Тестирование алгоритма AES (Legacy)" << std::endl;
        testSpeed("legacy", "AES-256-CTR");
}