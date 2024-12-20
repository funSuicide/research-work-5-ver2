#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../CtxFactory/CtxFactory.hpp"

class Server {
private:
    int serverFd;
    int port;
    void sendFile(int client_socket, unsigned char* key, unsigned char* iv);
    CtxFactory F;
public:
    Server(int port, char* prov, char* alg);
    void start(unsigned char* key, unsigned char* iv);
};

