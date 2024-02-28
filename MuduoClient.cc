#include <muduo/base/Logging.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

class MuduoClient
{
public:
    MuduoClient(muduo::net::EventLoop *loop, const muduo::net::InetAddress &serverAddr)
        : loop_(loop), client_(loop, serverAddr, "MuduoClient")
    {
        client_.setConnectionCallback(std::bind(&MuduoClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(std::bind(&MuduoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void connect()
    {
        client_.connect();
    }

    void sendMessage(const std::string &message)
    {
        // 使用锁确保线程安全
        std::lock_guard<std::mutex> lock(mutex_);
        auto connection = client_.connection();
        if (connection)
        {
            connection->send(message);
        }
        else
        {
            LOG_ERROR << "Connection is not valid";
        }
    }

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "Connected to server";
        }
        else
        {
            LOG_INFO << "Disconnected from server";
            LOG_INFO << "Failure reason: " << conn->getTcpInfoString();
            loop_->quit();
        }
    }

    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time)
    {
        std::string msg(buf->retrieveAllAsString());
        LOG_INFO << "Received " << msg.size() << " bytes from server: " << msg;
    }

    muduo::net::EventLoop *loop_;
    muduo::net::TcpClient client_;
    std::mutex mutex_;
};

void messageSender(MuduoClient &client)
{
    std::string message;
    while (std::getline(std::cin, message))
    {
        client.sendMessage(message);
    }
}

int main()
{
    muduo::net::EventLoop loop;
    // 更换成你自己的id port
    muduo::net::InetAddress serverAddr("192.168.3.45", 9877);

    MuduoClient client(&loop, serverAddr);
    client.connect();

    // 启动一个新线程来处理消息发送
    std::thread senderThread(messageSender, std::ref(client));

    loop.loop(); // 启动事件循环

    senderThread.join(); // 等待消息发送线程结束

    return 0;
}
