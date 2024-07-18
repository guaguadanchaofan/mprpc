#include <iostream>
#include <string>
#include "user.pb.h"
#include"mprpcapplication.h"
#include "rpcprovider.h"
class UserService : public fixbug::UserServiceRpc
{
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service :  Login" << std::endl;
        std::cout << "name: " << name << "pwd" << pwd << std::endl;
        return true;
    }

    //重写基类userservicerpc的虚函数 下面方法这个都是框架直接调用的
    virtual void Login(::google::protobuf::RpcController *controller,
                       const ::fixbug::LoginRequest *request,
                       ::fixbug::LoginResponse *response,
                       ::google::protobuf::Closure *done)
    {
        //框架给业务上报了请求参数LoginRequest，业务获取响应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();
        //做本地业务
        bool login_result = Login(name,pwd);

        //把响应写入 错误码 错误信息 返回值
        fixbug::ResultCode *code =response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        //执行回调执行 执行响应对象数据的序列化和网络发送（框架完成）
        done->Run();
    }
};


int main(int argc ,char **argv)
{
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    //把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    //启动一个rpc服务发布节点  Run以后进程阻塞等待r远程pc请求
    provider.Run();
    return 0;
} 