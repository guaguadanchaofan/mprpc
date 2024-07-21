#pragma once
#include <google/protobuf/service.h>

class MpRpcContrller : public google::protobuf::RpcController
{
public:
    MpRpcContrller();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string &reason);

    // 目前未实现
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);

private:
    bool _failed;         // rpc方法执行过程中的状态
    std::string _errText; // 调用的错误信息
};