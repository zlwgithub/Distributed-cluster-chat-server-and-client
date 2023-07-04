#include "Connection.h"
#include <iostream>
using namespace std;

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

Connection::Connection()
{
	_conn = mysql_init(nullptr);
}

Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port,
						 string username, string password, string dbname)
{
	MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
								  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
	if (p != nullptr)
	{
		// C和C++代码默认的编码字符是ASCII，如果不设置，从MySQL上拉下来的中文显示？
		mysql_query(_conn, "set names gbk");
		// LOG_INFO("connect mysql success!");
	}
	else
	{
		// LOG_INFO("connect mysql fail!");
	}

	return p;
}

bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		return false;
	}
	return true;
}

MYSQL_RES *Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		return nullptr;
	}
	return mysql_use_result(_conn);
}

// 获取连接
MYSQL *Connection::getConnection()
{
	return _conn;
}