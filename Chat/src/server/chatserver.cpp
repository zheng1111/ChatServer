#include "chatservice.hpp"
#include<string>
#include"json.hpp"
#include"chatserver.hpp"
using json=nlohmann::json;
ChatServer::ChatServer(EventLoop *loop,
            const InetAddress &listenAddr,
            const string &nameArg)
            :_server(loop,listenAddr,nameArg),_loop(loop)
    {
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
        _server.setThreadNum(4);
    }

void ChatServer::start(){
        _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr&conn){
    if(conn->connected()){
        std::cout<<"Online: "<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<std::endl;
    }else{
        std::cout<<"Offline: "<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<std::endl;
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr&conn,
                Buffer *buffer,
                Timestamp time)
{
    string buf=buffer->retrieveAllAsString();
    json js=json::parse(buf);
    // 达到目的，完全解耦网络模块和业务模块代码
    // 通过js["msgid"]获取handler -》 conn js time
    auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn,js,time);
}