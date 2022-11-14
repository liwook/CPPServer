#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>
#include<memory>
#include<iostream>
#include"src/ThreadPool.h"
using namespace std;


class ComptueServer
{
public:
    ComptueServer(EventLoop* loop, const InetAddr& listenAddr, int numThreads)
        : server_(listenAddr,loop)
        ,numThreads_(numThreads)
    {
        server_.setConnectionCallback([this](const ConnectionPtr& conn) {onConnection(conn); });
        server_.setMessageCallback([this](const ConnectionPtr& conn, Buffer* buf) {onMessage(conn,buf); });
    }

    void start()
    {
        threadPool_.start(numThreads_);
        server_.start();
    }

private:
    void onConnection(const ConnectionPtr& conn)
    {
        std::cout << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN")<<'\n';
    }

    void onMessage(const ConnectionPtr& conn, Buffer* buf)
    {
        threadPool_.run([&conn]() {
            long sum = 0;
            for (long i = 0; i < 1000000; ++i)
                sum += i;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            cout <<"threadPool "<< std::this_thread::get_id() << endl;
            conn->send(to_string(sum));
            });
    }

    Server server_;
    ThreadPool threadPool_;
    int numThreads_;
};

int main(int argc, char* argv[])
{
    EventLoop loop;
    InetAddr listenAddr(10000);
    ComptueServer server(&loop, listenAddr, 4);

    server.start();
    loop.loop();
    return 0;
}

