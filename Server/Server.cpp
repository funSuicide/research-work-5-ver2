#include "Server.hpp"
#include <chrono>
#include <vector>
#include <numeric>

using duration_t = std::chrono::duration<float>;

Server::Server(int port, char* prov, char* alg)
{
    this->port = port;
    this->F = CtxFactory(prov, alg);
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::sendFile(int client_socket, unsigned char* key, unsigned char* iv, const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        std::cerr << "file error: " << filename << std::endl;
        return;
    }

    OsslCtx c = F.next(key, iv);
    char buffer[4096];
    char buffer2[4096];
    
    while (file.read(buffer, sizeof(buffer))) {
        c.encrypt((unsigned char*)buffer, (unsigned char*)buffer2, file.gcount());
        send(client_socket, buffer, file.gcount(), 0);
    }
    
    if (file.gcount() > 0) {
        int tmp = c.encrypt((unsigned char*)buffer, (unsigned char*)buffer2, file.gcount()); 
        c.final_encrypt((unsigned char*)buffer2 + tmp);
        send(client_socket, buffer, file.gcount(), 0);
    }

    file.close();
}


void Server::start(unsigned char* key, unsigned char* iv, const char* filename) {
    std::vector<char *> files = {"test1", "test2","test3","test4","test5","test6","test7","test8", "test9", "test10"};
    std::vector<float> times(10);
    std::cout << "Сервер слушает порт: " << port << std::endl;
    int i = 0;
    while (true) {
        if (i > 9)
        {
            float sum = std::accumulate(times.begin(), times.end(), 0.0f);
            std::cout << "RESULT: " << 56320 / sum / 1024 / 1024 << std::endl;
            return ;
        }
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(serverFd, (struct sockaddr *)&address, &addrlen);
        
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Подключился новый клиент" << std::endl;
        auto begin = std::chrono::steady_clock::now();
        sendFile(client_socket, key, iv, files[i]);
        auto end = std::chrono::steady_clock::now();
        auto time = std::chrono::duration_cast<duration_t>(end - begin);
        times.push_back(time.count());
        std::cout << "Время работы: " << time.count() << std::endl;
        std::cout << "Скорость работы: " << 1 / time.count() << "ГБ/с" << std::endl;
        close(client_socket);
        std::cout << "Файл отправлен" << std::endl;
        i++;
    }

    
}
