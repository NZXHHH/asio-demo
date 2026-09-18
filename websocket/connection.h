#pragma once

#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <queue>
#include <mutex>
namespace net=boost::asio;
namespace beast=boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;
class Connection
{
public:
    Connection(net::io_context &ioc);
    void Start();
    net::ip::tcp::socket& GetSocket();
    std::string &GetUUID();
    void Read();
    void Write(const std::string &msg);
    void SendMsg(std::string msg);
private:
    std::unique_ptr<websocket::stream<tcp_stream>> _ws_ptr;
    std::string _uuid;
    std::queue<std::string> _sendQueue;
    std::mutex _mutex;
    flat_buffer _recv_buffer;
};