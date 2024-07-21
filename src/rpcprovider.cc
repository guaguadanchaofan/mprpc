#include "rpcprovider.h"
#include "rpcheader.pb.h"
// 这里是框架提供的外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    // 获取对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务名字
    std::string service_name = pserviceDesc->name();
    // 获取服务对象service的方法的数量
    int methodcnt = pserviceDesc->method_count();

    std::cout << "service_name :" << service_name << std::endl;
    LOG_INFO("service_name : %s",service_name.c_str());
    for (int i = 0; i < methodcnt; ++i)
    {
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info._methodMap.insert({method_name, pmethodDesc});
        LOG_INFO("method_name : %s",method_name.c_str());

    }
    service_info._service = service;
    _serviceMap.insert({service_name, service_info});
}

// 启动rpc服务节点，开始提供rpc远程调用服务
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    // 创建inetaddress对象
    muduo::net::InetAddress address(ip, port);
    // 创建TcpServer对象
    muduo::net::TcpServer server(&_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调   分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);
    LOG_INFO("RpcProvider start service at IP : %s  Port:%d ",ip.c_str(),port);

    // 启动网络服务
    server.start();
    _eventLoop.loop();
}

// 新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client连接断开
        conn->shutdown();
    }
}

// 已建立的连接用户的读写回调事件，如果远程有一个rpc调用请求 那么onmessag方法就会效应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp)
{
    // 网络上接受远程rpc调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();
    // 从字符流中读取前4个字节内容
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);
    // 根据header_size读取数据头的原始字符流
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcheader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcheader.ParseFromString(rpc_header_str))
    {
        // 反序列化成功
        service_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.args_size();
    }
    else
    {
        // 反序列化失败
        LOG_ERROR("rpc_header_str: %s  parse error",rpc_header_str.c_str());
        return;
    }
    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);
    // 打印调试信息
    std::cout << "=====================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "=====================================" << std::endl;

    // 获取service对象和method对象
    auto it = _serviceMap.find(service_name);
    if (it == _serviceMap.end())
    {
        LOG_ERROR("%s  is not exist!",service_name.c_str());
        return;
    }
    // 获取服务对象
    auto itm = it->second._methodMap.find(method_name);
    if (itm == it->second._methodMap.end())
    {
        LOG_ERROR("%s:%s  is not exist!",service_name.c_str(),method_name.c_str());
        return;
    }
    google::protobuf::Service *service = it->second._service;       // 获取serice对象  new UserService
    const google::protobuf::MethodDescriptor *method = itm->second; // 获取method对象 Login
    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) // 从args_str中解析参数
    {
        LOG_ERROR("request parse error! content:%s",args_str.c_str());
    }
    // 获取响应对象
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                                                    const muduo::net::TcpConnectionPtr &,
                                                                    google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse,
                                                                                                 conn, response);

    // 在框架上根据远端rpc请求,调用对应的对象方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str)) // response进行序列化
    {
        // 序列化成功后
        conn->send(response_str); // 通过muduo发送数据
    }
    else
    {
        LOG_ERROR("serialize response_str error!");
    }
    conn->shutdown(); // 模拟http短连接服务 有rpcprovider关闭文件描述符断开连接
}
