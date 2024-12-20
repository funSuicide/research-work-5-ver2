#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../CtxFactory/CtxFactory.hpp"

class Client {
public:
    Client(const std::string& ip_address, int port, char* prov, char* alg);
    void connectAndReceive(unsigned char* key, unsigned char* iv);
private:
    int sock;
    std::string ip_address;
    int port;
    CtxFactory F;
};