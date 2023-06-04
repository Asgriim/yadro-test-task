cmake_minimum_required(VERSION 3.25)

project(yadro_test_task)

set(CMAKE_CXX_STANDARD 17)

set(EXEC_NAME tape_sorter)


include_directories(include)

add_executable(${EXEC_NAME} main.cpp)

file(GLOB sources src/*.cpp)

foreach(X IN LISTS sources)
    string(REGEX MATCH "[^/]*\\.cpp"  OUT ${X})
    message(${OUT})
    target_sources(${EXEC_NAME}  PUBLIC src/${OUT})
endforeach()