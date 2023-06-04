#include <iostream>
#include "tape_device_f.h"
#include "util.h"
#include "tape_sorter.h"
#include <filesystem>
#include "config_reader.h"
constexpr char* confFile = "../config";

int main(int argc, char *argv[])
{


    //
    /// initialization
    //
    if (!std::filesystem::exists(confFile))
    {
        std::cout << "config file not found" << "\n";
        return -1;
    }
    if (argc < 3)
    {
        std::cout << "not enough arguments" << "\n";
        return -1;
    }
    ConfigReader configReader;
    configReader.readConfig(confFile);
    std::string value = configReader.getValue("text_mode");

    if (value.empty())
    {
        std::cout << "textMode not specified" << "\n";
        return -1;
    }
    int64_t textMode = std::stoi(value);

    if (textMode == 0 && argc < 4)
    {
        std::cout << "not enough arguments" << "\n";
        return -1;
    }

    int64_t rlate = std::stoi(configReader.getValue("read_latency"));
    int64_t wLate = std::stoi(configReader.getValue("write_latency"));
    int64_t rewLate = std::stoi(configReader.getValue("rewind_latency"));
    int64_t memoryLimit = 16777216;
    value = configReader.getValue("memory_limit");
    if (!value.empty())
    {
        memoryLimit = std::stoi(value);
    }
    int16_t tmpTapeLimit = 256;
    value = configReader.getValue("tmp_tape_limit");
    if (!value.empty())
    {
        if (std::stoi(value) < 30000)
        {
            tmpTapeLimit = std::stoi(value);
        }
    }

    bool emulateLatency = false;
    value = configReader.getValue("emulate_latency");
    if (!value.empty())
    {
        emulateLatency = (value == "1");
    }

    std::string inFile = argv[1];
    std::string outFile = argv[2];
    std::string tmpInfile = inFile + "_t";
    std::string tmpOutFile = outFile + "_t";
    int64_t size;

    if (!std::filesystem::exists(inFile))
    {
        std::cout << "input file not found" << "\n";
        return -1;
    }

    if (textMode)
    {
        size = convertTextToBinary(inFile, tmpInfile);

    } else
    {
        tmpInfile = argv[1];
        tmpOutFile = argv[2];
        size = std::stoi(argv[3]);
    }

    createEmptyBinaryFile(outFile, 1, size);
    //
    /// end of initialization
    //
    TapeDeviceF inTape(tmpInfile,rlate,wLate,rewLate,size);
    TapeDeviceF outTape(tmpOutFile,rlate,wLate,rewLate,size);

    TapeSorter sorter;
    sorter.setTempTapeLimit(tmpTapeLimit);
    sorter.setByteMemoryLimit(memoryLimit);
    sorter.setEmulateLatency(emulateLatency);
    sorter.sortTapes(inTape,outTape);

    if (textMode)
    {
        convertBinaryToTxt(outFile,tmpOutFile);
        std::remove(tmpInfile.data());
        std::remove(tmpOutFile.data());
    }

    return 0;
}
