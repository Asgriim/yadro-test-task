#include <iostream>
#include <test.h>
#include <fstream>
#include <memory>
#include <cstdint>
#include "tape_device_f.h"
#include "util.h"
int main()
{

    std::string path = "../tmp";
    std::string  txtPath = "../aboba";
//    convertTextFile(txtPath,path);
    TapeDeviceF deviceF(path,123,132,456,32);
    int32_t a = 0;
    std::cout << deviceF.atStart() << "\n";
    deviceF.read(a);
    std::cout << a << "\n";

    deviceF.rewindRight(3);
    deviceF.read(a);
    std::cout << a << "\n";

}
