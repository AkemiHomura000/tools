cmake_minimum_required(VERSION 3.0.0)
project(json2yaml VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(yaml-cpp REQUIRED)

add_executable(json2yaml main.cpp)
target_link_libraries(json2yaml
  yaml-cpp
)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
