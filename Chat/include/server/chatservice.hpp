#ifndef CHATSERGVICE_H
#define CHATSERGVICE_H
#include<mutex>
#include <unordered_map>
#include<functional>
#include<muduo/net/TcpConnection.h>
#include<iostream>
#include<string>
#include"json.hpp"
#include<memory>
#include<userModel.hpp>
#include"friendmodel.hpp"
#include"redis.hpp"
#include"offlinemessagemodel.hpp"
#include"groupmodel.hpp"
using json=nlohmann::json;
using namespace std;
using namespace muduo;
using namespace muduo::net;
// 登录
// {"msgid":1,"id":15,"password":"666666"}
// {"msgid":1,"id":22,"password":"pwd"}
// {"msgid":1,"id":19,"password":"123456"}

// 发送消息
// {"msgid":5,"id":22,"from":"zhengyuxin","to":15,"msg":"hello!"}
// {"msgid":5,"id":15,"from":"li si","to":22,"msg":"hello world!"}

// 添加好友
// {"msgid":6,"id":22,"friendid":15}


// 处理消息的事件回调方法类型
using MsgHandler=std::function<void(const TcpConnectionPtr&conn,json &js,Timestamp)>;
// 聊天服务器业务类
class ChatService{
public:
    // 获取单例对象的接口函数
    static ChatService* instance();
    // 处理登录业务
    void login(const TcpConnectionPtr&conn,json &js,Timestamp);
    // 处理注册业务
    void reg(const TcpConnectionPtr&conn,json &js,Timestamp);
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgId);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr&conn);
    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 服务器异常，业务重置
    void reset();
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 处理登出业务
    void loginout(const TcpConnectionPtr &conn,json &js,Timestamp time);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid, string msg);
private:
    ChatService();
    // 存储对应业务处理方法
    unordered_map<int,MsgHandler>_msgHandlerMap;
    // 存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr>_userConnMap;
    // 定义互斥锁，保证_userConnMap安全
    mutex _connMutex;
    // 数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis操作对象
    Redis _redis;
};
#endif