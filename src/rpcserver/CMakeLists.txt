# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(../../middleware/mysql MYSQL_LIST)
aux_source_directory(../../middleware/redis REDIS_LIST)
aux_source_directory(../../middleware/protobuf PROTOBUF_LIST)

# 指定生成可执行文件
add_executable(RpcServer ${SRC_LIST} ${MYSQL_LIST} ${REDIS_LIST} ${PROTOBUF_LIST})

# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(RpcServer mysqlclient hiredis pthread mprpc muduo_net muduo_base  protobuf zookeeper_mt)