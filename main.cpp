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
//    return 0;
    TapeDeviceF deviceF(path,123,132,456,32);
    int32_t a = 0;
    while (deviceF.read(a))
    {
        std::cout << a << "\n";
        deviceF.goNext();
    }
//    deviceF.goPrev();
//    deviceF.write(227);
//    deviceF.read(a);
//    deviceF.rewindToEnd();
//    deviceF.goPrev();
//    deviceF.read(a);
//    std::cout << a << "\n";
    return 0;
}
