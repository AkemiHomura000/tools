
cmake_minimum_required (VERSION 3.1)
project(NodeSim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)   #设置C++标准。该命令将C++标准设置为必选，即在编译C++代码时，必须使用该标准

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
message("QTVer:" Qt${QT_VERSION_MAJOR})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)

#将指定目录添加到编译器的头文件搜索路径之下，指定的目录被解释成当前源码路径的相对路径。
include_directories(inc)
include_directories(thirdpkg)

FILE(GLOB_RECURSE SOURCE_FILES inc/*.h  src/*.cpp )
message("Add Success")
#message(${SOURCE_FILES})

# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# 设置动态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# 存放可执行软件的目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(NodeSim
        MANUAL_FINALIZATION  #MANUAL_FINALIZATION是一个C++11新特性，它允许在类的构造函数和析构函数之外提供其他方法来控制对象的生命周期。
        main.cpp
        ${SOURCE_FILES}
    )
endif()

target_link_libraries(NodeSim PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

#设置目标属性
set_target_properties(NodeSim PROPERTIES
    WIN32_EXECUTABLE FALSE  # 是否存在cmd窗口
)

#https://zhuanlan.zhihu.com/p/630034777
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NodeSim)
endif()


# TODO: 如有需要，请添加测试并安装目标。
