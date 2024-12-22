#include "Client.hpp"
#include <vector>
#include <string>
#include <numeric>

int main(int argc, char* argv[]) {
    std::vector<std::string> files(10);
    std::vector<float> times(10);
    files.push_back("test1");
    files.push_back("test2");
    files.push_back("test3");
    files.push_back("test4");
    files.push_back("test5");
    files.push_back("test6");
    files.push_back("test7");
    files.push_back("test8");
    files.push_back("test9");
    files.push_back("test10");
    Client client("127.0.0.1", 1234, argv[1], argv[2]); 

    for (size_t i = 0; i < 10; ++i)
    {
        times.push_back(client.connectAndReceive((unsigned char*)argv[3], (unsigned char*)argv[4], files[i].c_str()));
    }

    float sum = std::accumulate(times.begin(), times.end(), 0.0f);
    float average = sum / times.size();
    std::cout << "Result: " << 56320 / average << std::endl;
}