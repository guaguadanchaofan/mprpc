#pragma once
#include "lockqueue.h"
enum  LogLevel
{
    INFO, // 普通信息
    ERROR // 错误信息
};

class Logger
{
public:
    static Logger &GetInstance();
    void SetLogLevel(LogLevel level); // 设置日志级别
    void Log(std::string msg);        // 写日志
private:
    LogLevel _Level ;                   // 记录日志级别
    LockQueue<std::string> _LockQueue; // 日志缓冲队列

    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};

// 定义宏
#define LOG_INFO(Logformat, ...)                       \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::GetInstance();        \
        logger.SetLogLevel(INFO);            \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, Logformat, ##__VA_ARGS__); \
        logger.Log(buf);                               \
    } while (0)

#define LOG_ERROR(Logformat, ...)                      \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::GetInstance();        \
        logger.SetLogLevel(ERROR);           \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, Logformat, ##__VA_ARGS__); \
        logger.Log(buf);                               \
    } while (0)
