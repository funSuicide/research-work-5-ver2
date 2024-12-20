#include "CtxFactory.hpp"
#include <iostream>

int main()
{   
    unsigned char key[] = "012345678DEF0123456789ABCDEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFEF";
    unsigned char iv[] = "EFEFEFEFEFEFEFEF";
    unsigned char testdata[] = "Hello, world!";

    unsigned char out[32] = {};
    unsigned char out2[32] = {};

    std::cout << out << std::endl;

    CtxFactory C("gost", "kuznechikAVX2");

    OsslCtx c = C.next(key, iv);
    c.encrypt(testdata, out, 14);
    c.final_encrypt(out);

    std::cout << out << std::endl;
    std::cout << "! "<< std::endl;

    OsslCtx c2 = C.next(key, iv);
    c2.encrypt(out, out2, 14);
    c2.final_encrypt(out2);
    std::cout << out2 << std::endl;
    std::cout << "! "<< std::endl;
}