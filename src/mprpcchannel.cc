#include "mprpcchannel.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service(); // 获取服务描述
    std::string service_name = sd->name();
    std::string method_name = method->name();
    // 获取参数的序列化字符长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) // 序列化
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("Serialize args_str error!");
        return;
    }
    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    std::string rpc_header_str;
    uint32_t header_size = 0;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("Serialize rpcheader error!");
        return;
    }

    // 组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4)); // 请求头大小
    send_rpc_str += rpc_header_str;                               // 请求头数据
    // send_rpc_str.insert(4 + header_size, std::string((char *)&args_size, 4));
    send_rpc_str += args_str;




    // 打印调试信息
    std::cout << "=====================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=====================================" << std::endl;




    // 使用tcp 完成rpc方法的远程调用

    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        char buf[512] = {0};
        sprintf(buf,"create socket error!   errno:%d",errno);
        controller->SetFailed(buf);
        return;
    }


    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);


    if (-1 == connect(cfd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        char buf[512] = {0};
        sprintf(buf,"connect  error!   errno:%d",errno);
        controller->SetFailed(buf);
        close(cfd);
        return;
    }


    if (-1 == send(cfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        char buf[512] = {0};
        sprintf(buf,"send  error!   errno:%d",errno);
        controller->SetFailed(buf);
        close(cfd);
        return;
    }

    // 接受rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(cfd, &recv_buf, sizeof(recv_buf), 0)))
    {
        char buf[512] = {0};
        sprintf(buf,"recv  error!   errno:%d",errno);
        controller->SetFailed(buf);
        close(cfd);
        return;
    }
    // std::string response_str(recv_buf,recv_size);//将缓冲区字符写入到 response_str
    if (!response->ParseFromArray(recv_buf, recv_size)) // 解析string的内容  反序列化
    {
        char buf[512] = {0};
        sprintf(buf,"response Parse error!   errno:%s",recv_buf);
        controller->SetFailed(buf);
        close(cfd);
        return;
    }
    close(cfd);
}