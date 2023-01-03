#include <mymuduo/TcpServer.h>
#include <mymuduo/Logger.h>
#include <iostream>
#include <string>
#include <functional>

class EchoServer
{
public:
    EchoServer(EventLoop* loop, 
    const InetAddress& addr, 
    const std::string& name) 
    : loop_(loop),
    server_(loop, addr, name)
    {
        // 注册回调函数
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1)
        );
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        //设置适合loop的线程数量
        server_.setThreadNum(3);


    }
    void start()
    {
        server_.start();
    }
private:
    //建立连接或者断开的回调
    void onConnection(const TcpConnectionPtr& conn) 
    {
        if(conn->connected()) 
        {
            LOG_INFO("Connection UP : %s", conn->peerAddress().toIpPort().c_str());
        }
        else
        {
            LOG_INFO("Connection DOWN : %s", conn->peerAddress().toIpPort().c_str());
        }
    }

    //可读写的事件回调
    void onMessage(const TcpConnectionPtr& conn, 
    Buffer* buf, Stamptimer time)
    {
        std::string msg = buf->retrieveAllAsString();
        std::cout<<msg<<" "<<"this is a try"<<std::endl;
        conn->send(msg);
        conn->shutdown();
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main() 
{
    EventLoop loop;
    InetAddress addr(8000);
    EchoServer server(&loop, addr, "EchoServer-1");
    server.start();
    loop.loop();
    return 0;
}

