cmake_minimum_required(VERSION 3.0.0)
project(classic_examples_impl VERSION 0.1.0 LANGUAGES C CXX)

#支持c++20标准
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

add_executable(classic_examples_impl main.cpp)

target_link_libraries(${PROJECT_NAME} pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
