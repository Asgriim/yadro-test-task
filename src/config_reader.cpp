//
// Created by asgrim on 04.06.23.
//
#include <filesystem>
#include <fstream>
#include "config_reader.h"

bool ConfigReader::readConfig(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        return false;
    }
    std::string line;
    std::ifstream ifstream;
    ifstream.open(path);
    while (ifstream >> line)
    {
        int ind = line.find('=');
        if (ind == -1)
        {
            continue;
        }
        std::string key = line.substr(0,ind);
        std::string value = line.substr(ind + 1, line.size());
        m_confMap.insert({key,value});
    }
    return true;
}

std::string ConfigReader::getValue(const std::string & key) {

    auto it = m_confMap.find(key);
    if (it == m_confMap.end())
    {
        return std::string();
    }

    return it->second;
}
