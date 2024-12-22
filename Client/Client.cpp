#include "Client.hpp"
#include <chrono>

using duration_t = std::chrono::duration<float>;

Client::Client(const std::string& ip_address, int port, char* prov, char* alg)
{
    this->ip_address = ip_address;
    this->port = port;
    this->F = CtxFactory(prov, alg);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
}

float Client::connectAndReceive(unsigned char* key, unsigned char* iv, const char* filename) {
    struct sockaddr_in serv_addr;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("address error");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection error");
        exit(EXIT_FAILURE);
    }

    //const char *filename = "received_file";
    std::ofstream file(filename, std::ios::binary);

    OsslCtx c = F.next(key, iv);
    char buffer[4096];
    char buffer2[4096];
    ssize_t bytes_received;

    auto begin = std::chrono::steady_clock::now();
		
		
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        int tmp = c.encrypt((unsigned char*)buffer, (unsigned char*)buffer2, bytes_received);
        if (bytes_received < sizeof(buffer))
        {
            c.final_encrypt((unsigned char*)buffer2 + tmp);
        }
        file.write(buffer2, bytes_received);
    }

    auto end = std::chrono::steady_clock::now();

    if (bytes_received < 0) {
        perror("recv failed");
    } else {
        std::cout << "Файл успешно получен." << std::endl;
    }

    file.close();

    auto time = std::chrono::duration_cast<duration_t>(end - begin);
    std::cout << "Время работы: " << time.count() << std::endl;
    std::cout << "Скорость работы: " << 1 / time.count() << "ГБ/с" << std::endl;
}
