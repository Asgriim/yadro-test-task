//
// Created by asgrim on 04.06.23.
//

#ifndef YADRO_TEST_TASK_CONFIG_READER_H
#define YADRO_TEST_TASK_CONFIG_READER_H
#include <iostream>
#include "unordered_map"
class ConfigReader
{
public:
    bool readConfig(const std::string& path);
    std::string getValue(const std::string & key);

private:
    std::unordered_map<std::string, std::string> m_confMap;
};

#endif //YADRO_TEST_TASK_CONFIG_READER_H
