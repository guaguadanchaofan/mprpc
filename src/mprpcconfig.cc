#include "mprpcconfig.h"
#include <stdio.h>
#include <iostream>
#include <string>
void MprpcConfig::LoadConfigFile(const char *confif_file)
{
    FILE *pf = fopen(confif_file, "r");
    if (pf == nullptr)
    {
        std::cout << confif_file << "is not exist!" << std::endl;
        exit(0);
    }

    while (!feof(pf))
    {
        char buf[512] = {};
        fgets(buf, 512, pf);
        // 去掉字符串前面多余的空格
        std::string src_buf(buf);
        Trim(src_buf);
        // 判断#注释
        if (src_buf[0] == '#' || src_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = src_buf.find('=');
        if (idx == -1)
        {
            // 配置不合法
            continue;
        }
        std::string key;
        std::string value;
        key = src_buf.substr(0, idx);
        Trim(key);
        int endidx = src_buf.find('\n');
        value = src_buf.substr(idx + 1, endidx - idx - 1);
        _configMap.insert({key, value});
    }
}

std::string MprpcConfig::Load(std::string key)
{
    auto it = _configMap.find(key);
    if (it == _configMap.end())
    {
        return "";
    }
    return it->second;
}

void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx + 1);
    }
}
