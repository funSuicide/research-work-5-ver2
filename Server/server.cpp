#include "Server.hpp"

int main(int argc, char* argv[]) {
    Server server(1234, argv[1], argv[2]); 
    server.start((unsigned char*)argv[3], (unsigned char*)argv[4]);
}