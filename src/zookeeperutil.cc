#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

// 全局的watcher观察者
void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path,
                    void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh); // 和会话相关的回调的消息类型
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : _zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
    if (_zhandle != nullptr)
    {
        zookeeper_close(_zhandle); // 关闭句柄，释放资源
    }
}

void ZkClient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zeekeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zeekeeperport");
    std::string connstr = host + ":" + port;
    /*
    zookeeper_mt:多线程版本
    zookeeper的API客户端程序提供了三个线程
    API调用线程
    网络I/O线程 pthread_create poll
    watcher回调线程
    */
    _zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (_zhandle == nullptr)
    {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(-1);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(_zhandle, &sem);
    sem_wait(&sem);
    std::cout << "zookeeper_init success!" << std::endl;
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
    // 先判断path表示的znode节点是否存在，如果存在就不再重复创建
    flag = zoo_exists(_zhandle, path, 0, nullptr);
    if (ZNONODE == flag) // 表示节点不存在
    {
        // 创建指定的path的znode节点
        flag = zoo_create(_zhandle, path, data, datalen,
                          &ZOO_OPEN_ACL_UNSAFE, state, path_buffer,
                          bufferlen);
        if (flag == ZOK)
        {
            std::cout << "znode create success... path:" << path << std::endl;
        }
        else
        {
            std::cout << "flag: " << flag << std::endl;
            std::cout << " znode create error... path:" << path << std::endl;
            exit(-1);
        }
    }
}

// 根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK)
    {
        std::cout << "get znode error...path:" << path << std::endl;
        return "";
    }
    else
    {
        return buffer;
    }
}