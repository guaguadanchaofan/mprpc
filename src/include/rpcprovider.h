#pragma once
#include"google/protobuf/service.h"

//框架提供的专门服务发布的rpc服务对象类
class RpcProvider
{
public:
    //这里是框架提供的外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* service );//发布服务
    //启动rpc服务节点，开始提供rpc远程调用服务
    void Run();
};