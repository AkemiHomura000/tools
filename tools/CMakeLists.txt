cmake_minimum_required(VERSION 3.0.0)
project(json2yaml VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(yaml-cpp REQUIRED)

add_executable(json2yaml main.cpp)
target_link_libraries(json2yaml
  yaml-cpp
)
add_executable(pgm2jpg pgm2jpg.cpp)
target_link_libraries(pgm2jpg ${OpenCV_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
