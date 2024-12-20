#include <iostream>
#include <fstream>
#include <random>

void generateRandomFile(const std::string& filename, size_t size_in_bytes) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return;
    }

    std::random_device rd;  
    std::mt19937 generator(rd()); 
    std::uniform_int_distribution<unsigned char> distribution(0, 255); 

    const size_t buffer_size = 4096; 
    char buffer[buffer_size];

    for (size_t i = 0; i < size_in_bytes; i += buffer_size) {
        size_t bytes_to_write = std::min(buffer_size, size_in_bytes - i);
        for (size_t j = 0; j < bytes_to_write; ++j) {
            buffer[j] = distribution(generator); 
        }
        file.write(buffer, bytes_to_write); 
    }

    file.close();
    std::cout << "File generated: " << filename << " (" << size_in_bytes << " bytes)" << std::endl;
}

int main() {
    const std::string filename = "file_to_send"; 
    const size_t size_in_gb = 1; 
    const size_t size_in_bytes = size_in_gb * 1024 * 1024 * 1024; 

    generateRandomFile(filename, size_in_bytes); 

    return 0;
}
