//
// Created by asgrim on 03.06.23.
//
#include <fstream>
#include <iostream>
#include "util.h"

bool convertBinaryFile(const std::string &txtFilePath, const std::string &binFilePath)
{
    std::FILE *binaryFile;
    std::ofstream textStream;

    binaryFile = std::fopen(binFilePath.data(), "rb");
    textStream.open(txtFilePath);

    if(binaryFile == NULL || textStream.bad())
    {
        fclose(binaryFile);
        textStream.close();
        return false;
    }
    int32_t t;
    while (!std::feof(binaryFile))
    {
        if(std::fread(&t, sizeof(t), 1, binaryFile) == 1)
        {
            textStream << t << "\n";
        } else
        {
            break;
        }
    }
    std::fclose(binaryFile);
    textStream.close();
    return true;
}

bool convertTextFile(const std::string &txtFile, const std::string &binFile)
{
    std::ofstream binaryStream;
    std::ifstream textStream;
    binaryStream.open(binFile);
    textStream.open(txtFile);

    if(binaryStream.bad() || textStream.bad())
    {
        binaryStream.close();
        textStream.close();
        return false;
    }

    int32_t tmp;
    while (textStream >> tmp)
    {
        binaryStream.write((char*)&tmp, sizeof(tmp));
    }
    binaryStream.close();
    textStream.close();
    return true;
}

bool createEmptyBinaryFile(const std::string &binFilePath, const int64_t elementByteSize, const int64_t count)
{
    std::FILE *binaryFile;
    binaryFile = std::fopen(binFilePath.data(), "wb");

    if (binaryFile == NULL)
    {
        return false;
    }
    int8_t t;
    int64_t n = elementByteSize * count;
    if (std::fwrite(&t, sizeof(t), n,binaryFile) != n)
    {
        fclose(binaryFile);
        return false;
    }
    fclose(binaryFile);
    return true;

}
