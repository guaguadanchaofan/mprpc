#include "logger.h"
#include <time.h>
#include <iostream>
Logger &Logger::GetInstance()
{
    static Logger logger; // 获取实例
    return logger;
}

void Logger::SetLogLevel(LogLevel level)
{
    _Level = level;
}

// 写日志 把信息写入到LockQueue缓冲区
void Logger::Log(std::string msg)
{
    _LockQueue.Push(msg);
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&]()
                             {
        for(;;)
        {
            //获取当前日期，然后取日志信息然后写入
            time_t now = time(nullptr);
            tm* tm = localtime(&now);

            char file_name[128] = {0};
            sprintf(file_name,"%d-%d-%d_log.txt",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);
            FILE* fp = fopen(file_name,"a+");
            if(fp==nullptr)
            {
                std::cout<<"fopen : "<<file_name<<" is error! "<<std::endl;
                exit(0);
            }
            std::string msg = _LockQueue.Pop();
            char buf[128] = {0};
            sprintf(buf,"[%02d:%02d:%02d][%s]",
                    tm->tm_hour,
                    tm->tm_min,
                    tm->tm_sec,
                    _Level== INFO ? "INFO":"ERROR");
            msg.insert(0,buf);
            msg.append("\n");
            fputs(msg.c_str(),fp);
            fclose(fp);
        } });
    // 设置分离线程 守护线程
    writeLogTask.detach();
}
