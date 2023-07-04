# Distributed-cluster-chat-server-and-client
分布式集群网络社交聊天服务器和客户端源码  

项目名称：分布式集群网络社交聊天服务器和客户端 
项目简介: 
整个项目使用 C++在 Linux 环境下开发。服务器分为连接服务器和业务服务器，可采用分布式集群系统设计
部署。连接服务器负责接收并保存与客户端的连接，并通过业务服务器提供的功能与客户端进行交互。根据业务
的侧重点(CPU 密集和 IO 密集)可将业务模块代码分别在不同的服务器上实现，业务服务器按功能分为用户模块、
好友模块、群组模块、消息模块、日志模块。客户端基于 nginx 负载均衡选择合适的连接服务器进行登录。连接
服务器底层使用 muduo 网络库接收客户端的连接和请求，根据客户端的请求使用 mprpc 框架远程调用对应该业务
模块的服务器的方法，并将结果返回给客户端。整个项目的网络通信模块均使用 protobuf 作为数据序列化和反序
列化的工具。对于在不同服务器上登录的用户，使用了 redis 发布与订阅技术进行聊天内容转发。 


项目技术栈： 
1、Muduo 网络库：客户端和连接服务器的通信以及 mprpc 框架的实现都使用了自实现的 muduo 网络库。 
2、mprpc 分布式服务器通信框架：基于 muduo 网络库、Protobuf 以及 Zookeeper 注册中心编写，可用于分布式
服务器模块间的通信。mprpc 根据连接服务器请求的业务向 zookeeper 注册中心查询该业务模块服务器的 ip 和
端口，底层使用 muduo 网络库建立与业务服务器间的连接并用 protobuf 交换数据。 
3、protobuf：客户端和连接服务器的交互使用传输效率更高的二进制序列化框架 protobuf 进行通信。 
4、连接池：登录服务器使用了 rpc 连接池技术；所有服务器均使用 mysql 连接池技术。 
5、中间件的使用：nginx 负载均衡、redis 发布与订阅、mysql 作为服务器数据库。 
