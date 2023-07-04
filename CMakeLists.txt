# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(QQ)

# 生成debug版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 配置头文件的搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/chatclient)
include_directories(${PROJECT_SOURCE_DIR}/include/connectserver)
include_directories(${PROJECT_SOURCE_DIR}/include/rpcserver)
include_directories(${PROJECT_SOURCE_DIR}/middleware)
include_directories(${PROJECT_SOURCE_DIR}/middleware/mysql)
include_directories(${PROJECT_SOURCE_DIR}/middleware/protobuf)
include_directories(${PROJECT_SOURCE_DIR}/middleware/redis)

# 设置项目库文件搜索路径 -L
link_directories(/usr/lib)
link_directories(/usr/lib64/mysql)

# 加载子cmake目录
# 包含框架所有代码
add_subdirectory(src)

# add_subdirectory(test)