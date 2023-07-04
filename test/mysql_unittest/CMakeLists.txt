# 设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mysql_unittest)

# 生成debug版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)


# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(../../middleware/mysql MYSQL_LIST)

# 指定生成可执行文件
add_executable(mysql_unittest ${SRC_LIST} ${MYSQL_LIST})

include_directories(../../middleware/mysql)

# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(mysql_unittest mysqlclient pthread)