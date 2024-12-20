#include "Server.hpp"
#include <chrono>

using duration_t = std::chrono::duration<float>;

Server::Server(int port, char* prov, char* alg)
{
    this->port = port;
    this->F = CtxFactory(prov, alg);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::sendFile(int client_socket, unsigned char* key, unsigned char* iv) {
    const char *filename = "file_to_send";
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return;
    }

    OsslCtx c = F.next(key, iv);
    char buffer[4096];
    char buffer2[4096];
    
    // Читаем файл и отправляем его клиенту
    while (file.read(buffer, sizeof(buffer))) {
        c.encrypt((unsigned char*)buffer, (unsigned char*)buffer2, file.gcount());
        send(client_socket, buffer2, file.gcount(), 0);
    }
    
    // Отправляем оставшиеся данные
    if (file.gcount() > 0) {
        int tmp = c.encrypt((unsigned char*)buffer, (unsigned char*)buffer2, file.gcount()); 
        c.final_encrypt((unsigned char*)buffer2 + tmp);
        send(client_socket, buffer2, file.gcount(), 0);
    }

    file.close();
}

void Server::start(unsigned char* key, unsigned char* iv) {
    std::cout << "Server is listening on port " << port << std::endl;

    while (true) {
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Client connected" << std::endl;
        sendFile(client_socket, key, iv);
        close(client_socket);
        std::cout << "File sent and client disconnected" << std::endl;
    }
}
