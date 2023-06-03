//
// Created by asgrim on 03.06.23.
//

#ifndef YADRO_TEST_TASK_UTIL_H
#define YADRO_TEST_TASK_UTIL_H
#include <string>

bool convertBinaryFile(const std::string &txtFilePath, const std::string &binFilePath);
bool convertTextFile(const std::string &txtFile, const std::string &binFile);
bool createEmptyBinaryFile(const std::string &binFilePath, const int64_t elementByteSize, const int64_t count);

#endif //YADRO_TEST_TASK_UTIL_H
