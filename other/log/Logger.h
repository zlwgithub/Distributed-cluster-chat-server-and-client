#pragma once
#include "lockqueue.h"
#include <string>
#include <mymuduo/noncopyable.h>

// LOG_INFO("xxx%sxxxxx%dxxx", arg1, arg2)
#define LOG_INFO(logmsgFormat, ...)                                     \
    do                                                                  \
    {                                                                   \
        Logger &logger = Logger::GetInstance();                         \
        logger.setLogLevel(ERROR);                                      \
        char buf[1024] = "[INFO]";                                      \
        snprintf(buf + strlen(buf), 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf);                                                \
    } while (0)

#define LOG_ERROR(logmsgFormat, ...)                                    \
    do                                                                  \
    {                                                                   \
        Logger &logger = Logger::GetInstance();                         \
        logger.setLogLevel(ERROR);                                      \
        char buf[1024] = "[ERROR]";                                     \
        snprintf(buf + strlen(buf), 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf);                                                \
    } while (0)

#define LOG_FATAL(logmsgFormat, ...)                                    \
    do                                                                  \
    {                                                                   \
        Logger &logger = Logger::GetInstance();                         \
        logger.setLogLevel(FATAL);                                      \
        char buf[1024] = "[FATAL]";                                     \
        snprintf(buf + strlen(buf), 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf);                                                \
        exit(EXIT_FAILURE);                                             \
    } while (0)

#ifdef RPCDEBUG
#define LOG_DEBUG(logmsgFormat, ...)                                    \
    do                                                                  \
    {                                                                   \
        Logger &logger = Logger::GetInstance();                         \
        logger.setLogLevel(DEBUG);                                      \
        char buf[1024] = "[DEBUG]";                                     \
        snprintf(buf + strlen(buf), 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.Log(buf);                                                \
    } while (0)
#else
#define LOG_DEBUG(logmsgFormat, ...)
#endif

// 定义日志的级别  INFO  ERROR  FATAL  DEBUG
enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL, // core信息
    DEBUG, // 调试信息
};

// Mprpc框架提供的日志系统
class Logger : noncopyable
{
public:
    // 获取日志的单例
    static Logger &GetInstance();
    //  设置日志级别
    void setLogLevel(LogLevel level);
    // 写日志到lockqueue缓冲区中
    void Log(std::string msg);

private:
    LogLevel m_loglevel;              // 记录日志级别
    LockQueue<std::string> m_lockQue; // 日志缓存队列

    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};
