#pragma once
#include "mprpcapplication.h"
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <functional>
#include <unordered_map>
#include"logger.h"
// 框架提供的专门服务发布的rpc服务对象类
class RpcProvider
{
public:
    // 这里是框架提供的外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service); // 发布服务
    // 启动rpc服务节点，开始提供rpc远程调用服务
    void Run();

private:
    // 新的socket连接回调  消息回调
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    // 已建立的连接用户的读写回调事件，如果远程有一个rpc调用请求 那么onmessag方法就会效应
    void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    // 响应回调 Closure的回调操作 用于序列化rpc响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);
    // 组合EventLoop
    muduo::net::EventLoop _eventLoop;

    // service服务类型信息
    struct ServiceInfo
    {
        // 保存的服务对象
        google::protobuf::Service *_service;
        // 保存服务的方法
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> _methodMap;
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> _serviceMap;
};