#pragma once
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include"mprpcontrller.h"
// mprpc框架的初始化类（提供初始化）
class MprpcApplication // 单例
{
public:
    static void Init(int argc, char **argv); // 初始化
    static MprpcApplication &GetInstance();  // 获取实例
    static MprpcConfig &GetConfig();

private:
    static MprpcConfig _config;
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};