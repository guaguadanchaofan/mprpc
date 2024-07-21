#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <config_file>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(0);
    }
    std::string config_file(argv[2]);
    int c = 0;
    if (c = getopt(argc, argv, "i:") != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(0);
        case ':':
            ShowArgsHelp();
            exit(0);
        default:
            break;
        }
    }
    // 开始加载配置文件
    _config.LoadConfigFile(config_file.c_str());

    // std::cout<<"rpcserverip: "<<_config.Load("rpcserverip")<<std::endl;
    // std::cout<<"rpcserverport: "<<_config.Load("rpcserverport")<<std::endl;
    // std::cout<<"zeekeeperip: "<<_config.Load("zeekeeperip")<<std::endl;
    // std::cout<<"zeekeeperport: "<<_config.Load("zeekeeperport")<<std::endl;
}

MprpcApplication &MprpcApplication::GetInstance() // 获取实例
{
    static MprpcApplication app;
    return app;
}

MprpcConfig &MprpcApplication::GetConfig()
{
    return _config;
}
