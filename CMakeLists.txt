#cmake最低版本需求，不加入此行会受到警告信息
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

#项目名称
PROJECT(csgo_cheater) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
AUX_SOURCE_DIRECTORY(. SRC_LIST)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

#生成应用程序
ADD_EXECUTABLE(csgo_cheater ${SRC_LIST})

#cmake .. -G "Visual Studio 17 2022" -A Win32