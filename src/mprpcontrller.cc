#include "mprpcontrller.h"

MpRpcContrller::MpRpcContrller()
{
    _failed = false;
    _errText = "";
}

void MpRpcContrller::Reset()
{
    _failed = false;
    _errText = "";
}

bool MpRpcContrller::Failed() const
{
    return _failed;
}

std::string MpRpcContrller::ErrorText() const
{
    return _errText;
}

void MpRpcContrller::SetFailed(const std::string &reason)
{
    _errText = reason;
    _failed = true;
}

// 目前未实现
void MpRpcContrller::StartCancel() {}
bool MpRpcContrller::IsCanceled() const {}
void MpRpcContrller::NotifyOnCancel(google::protobuf::Closure *callback) {}
