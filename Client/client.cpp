#include "Client.hpp"

int main(int argc, char* argv[]) {
    Client client("127.0.0.1", 1234, argv[1], argv[2]); 
    client.connectAndReceive((unsigned char*)argv[3], (unsigned char*)argv[4]);
}