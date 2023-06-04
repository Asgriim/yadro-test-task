#include <iostream>
#include <test.h>
#include <fstream>
#include <memory>
#include <cstdint>
#include "tape_device_f.h"
#include "util.h"
#include "tape_sorter.h"
#include "vector"
#include <algorithm>
#include <queue>
#include <filesystem>
constexpr char* confFile = "./config";

int main()
{

    if (!std::filesystem::exists(confFile))
    {
        std::cout << "config file not found";
        return -1;
    }

    std::string path = "../tmp";
    std::string path2 = "../out2t";
    std::string txtPath = "../aboba";
    std::string bebra = "../normalSorted";
    std::string test = "../test";
    std::string tmpTestTape = "/tmp/tempTape43";
    convertTextToBinary(txtPath,path);
    TapeSorter sorter;
    TapeDeviceF deviceF(path,456,456,456, 216);
    TapeDeviceF deviceF2(path2,456,456,456, 216);
    sorter.setByteMemoryLimit(8);
    sorter.setTempTapeLimit(1000);
    sorter.setRealLAtency(true);
    sorter.sortTapes(deviceF,deviceF2);
    convertBinaryToTxt(test, path2);
//    convertBinaryToTxt(test,tmpTestTape);
    return 0;
}
