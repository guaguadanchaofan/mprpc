#include "test.pb.h"
#include<iostream>
#include<string>
using namespace fixbug;
int main()
{
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败了");

    GetFriendListResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(1);
    
    User * user1 = rsp.add_friend_list(); 
    user1->set_age(20);
    user1->set_name("zhangsan");
    user1->set_sex(User::MAN);

    std::cout<<rsp.friend_list_size()<<std::endl;

    // LoginRequest req;
    // req.set_name("zhang san");
    // req.set_pwd("12345");
    // std::string send_str;
    // //序列化
    // if(req.SerializeToString(&send_str))
    // {
    //     std::cout<<send_str<<std::endl;
    // }

    // //反序列化
    // LoginRequest reqb;
    // if(reqb.ParseFromString(send_str))
    // {
    //     std::cout<<reqb.name()<<std::endl;
    //     std::cout<<reqb.pwd()<<std::endl;
    // }
    return 0;
}