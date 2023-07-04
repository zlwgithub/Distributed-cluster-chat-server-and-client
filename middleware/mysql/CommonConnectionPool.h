#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
using namespace std;
#include "Connection.h"

/*
ʵ�����ӳع���ģ��
*/
class ConnectionPool
{
public:
	// ��ȡ���ӳض���ʵ��
	static ConnectionPool *getConnectionPool();
	// ���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();

private:
	// ����#1 ���캯��˽�л�
	ConnectionPool();

	// �������ļ��м���������
	bool loadConfigFile();

	// �����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	// ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ����ӻ���
	void scannerConnectionTask();

	string _ip;				// mysql��ip��ַ
	unsigned short _port;	// mysql�Ķ˿ں� 3306
	string _username;		// mysql��¼�û���
	string _password;		// mysql��¼����
	string _dbname;			// ���ӵ����ݿ�����
	int _initSize;			// ���ӳصĳ�ʼ������
	int _maxSize;			// ���ӳص����������
	int _maxIdleTime;		// ���ӳ�������ʱ��
	int _connectionTimeout; // ���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection *> _connectionQue; // �洢mysql���ӵĶ���
	mutex _queueMutex;					// ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCnt;			// ��¼������������connection���ӵ�������
	condition_variable cv;				// ���������������������������̺߳����������̵߳�ͨ��
};