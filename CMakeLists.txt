cmake_minimum_required(VERSION 3.0.0)
project(classic_examples_impl VERSION 0.1.0 LANGUAGES C CXX)

#支持c++20标准
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

#查找Qt库
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)  

add_executable(classic_examples_impl main.cpp)

# 添加头文件路径  
include_directories(  
    ${CMAKE_CURRENT_SOURCE_DIR}  
)  

target_link_libraries(${PROJECT_NAME} pthread Qt5::Core Qt5::Gui Qt5::Widgets)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
