#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include <iostream>

class MuduoServer
{
public:
    MuduoServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr) : server_(loop, listenAddr, "MuduoServer")
    {
        server_.setConnectionCallback(std::bind(&MuduoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&MuduoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void start()
    {
        server_.start();
    }

private:
    muduo::net::TcpServer server_;

    void onConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            std::cout << "Client connected: " << conn->peerAddress().toIpPort() << std::endl;
        }
        else
        {
            std::cout << "Client disconnected: " << conn->peerAddress().toIpPort() << std::endl;
        }
    }

    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time)
    {
        std::string msg(buf->retrieveAllAsString());
        std::cout << "Received " << msg.size() << " bytes from " << conn->peerAddress().toIpPort() << std::endl;
        conn->send(msg);
    }
};

int main()
{
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(9877);

    MuduoServer server(&loop, listenAddr);

    server.start();
    loop.loop();

    return 0;
}