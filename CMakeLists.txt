cmake_minimum_required(VERSION 3.25)

project(yadro_test_task)

set(CMAKE_CXX_STANDARD 17)


include_directories(include)


add_executable(${PROJECT_NAME} main.cpp)

file(GLOB sources src/*.cpp)

foreach(X IN LISTS sources)
    string(REGEX MATCH "[^/]*\\.cpp"  OUT ${X})
    message(${OUT})
    message(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PUBLIC src/${OUT})
endforeach()