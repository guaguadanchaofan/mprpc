#include<iostream>
#include"mprpcapplication.h"
#include"mprpcchannel.h"
#include"user.pb.h"
int main(int argc, char** argv)
{
    //整个程序启动以后，想使用mprpc框架需要进行初始化（初始化一次）
    MprpcApplication::Init(argc,argv);

    //
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    //stub.Login() ;//RpcChannel->RpcChannel::callMthod 集中来做所有rpc方法调用的参数序列化和网络发送

    return 0;
}