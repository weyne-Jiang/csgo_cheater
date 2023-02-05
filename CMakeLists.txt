#cmake最低版本需求，不加入此行会受到警告信息
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

#项目名称
project(csgo_cheater)
   
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set (CMAKE_CXX_FLAGS "-m32")
#set (CMAKE_C_FLAGS "-m32") 

#把当前目录(.)下所有源代码文件和头文件加入变量
aux_source_directory(${PROJECT_SOURCE_DIR}/Src src_list)

#添加宏定义，unicode编码
#add_definitions("-DUNICODE" "-D_UNICODE")

#IDE分组
file(GLOB_RECURSE Include ${PROJECT_SOURCE_DIR}/Include *.h *.hpp)
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${Include})

#自己的头文件目录
include_directories(${PROJECT_SOURCE_DIR}/Include/DirectX_SDK)
include_directories(${PROJECT_SOURCE_DIR}/Include/)

#自己的库文件目录
link_directories(${PROJECT_SOURCE_DIR}/Lib/DirectX_SDK/x86)

#生成应用程序
add_executable(csgo_cheater ${src_list} ${Include})

#添加链接库
target_link_libraries(csgo_cheater d3d9.lib d3dx9.lib d3dx9d.lib dwmapi.lib)

#message(${SRC_LIST});
#cmake .. -G "Visual Studio 17 2022" -A Win32