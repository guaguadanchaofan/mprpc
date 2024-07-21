#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"

int main(int argc, char **argv)
{
    // 整个程序启动以后，想使用mprpc框架需要进行初始化（初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布rpc发的Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // stub.Login() ;//RpcChannel->RpcChannel::callMthod 集中来做所有rpc方法调用的参数序列化和网络发送
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    fixbug::LoginResponse response;
    MpRpcContrller contrller;

    // 发布rpc方法的调用 同步rpc调用过程
    stub.Login(&contrller, &request, &response, nullptr);
    if (contrller.Failed())
    {
        std::cout<<contrller.ErrorText()<<std::endl;
    }
    else
    {
        if (0 == response.result().errcode()) // 调用完成 读取效应结果
        {
            std::cout << "rpc longin response success : " << response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error : " << response.result().errcode() << std::endl;
            // exit(0);
        }
    }

    return 0;
}