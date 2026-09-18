#include "connection.h"
#include "ConnectionMgr.h"
Connection::Connection(net::io_context &ioc):_ws_ptr(std::make_unique<websocket::stream<tcp_stream>>(ioc))
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(uuid);
}

void Connection::Start()
{
    _ws_ptr->async_accept([this](const boost::system::error_code &ec)
    {
        if (!ec)
        {
            std::cout << "WebSocket connection established. UUID: " << _uuid << std::endl;
            Read();
        }
        else
        {
            std::cerr << "WebSocket accept error: " << ec.message() << std::endl;
        }
    });
}

net::ip::tcp::socket &Connection::GetSocket()
{
    auto &socket = boost::beast::get_lowest_layer(*_ws_ptr).socket();
    return socket;
}

std::string &Connection::GetUUID()
{
    return _uuid;
}
void Connection::Read()
{
    _ws_ptr->async_read(_recv_buffer, [this](const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::string message = boost::beast::buffers_to_string(_recv_buffer.data());
            std::cout << "Received: " << message << std::endl;
            _recv_buffer.consume(bytes_transferred);
            Write(std::move(message));
            
        }
        else
        {
            ConnectionMgr::GetInstance()->RemoveConnection(_uuid);
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
        Read();
       });
}
void Connection::Write(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _sendQueue.push(msg);
    if (_sendQueue.size() > 1)
    {
        return; // A write operation is already in progress
    }
    else
    {
        SendMsg(std::move(msg));
    }
}
void Connection::SendMsg(std::string msg)   
{
    _ws_ptr->async_write(net::buffer(msg.c_str(), msg.length()), [this, msg](const boost::system::error_code &ec, std::size_t bytes_transferred)
    {
        if (!ec)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            std::cout << "Sent: " << msg << std::endl;
            _sendQueue.pop(); // Remove the sent message from the queue
            if(!_sendQueue.empty())
            {
                auto next_msg = _sendQueue.front();
                SendMsg(std::move(next_msg)); // Send the next message in the queue
            }
        }
        else
        {
            std::cerr << "Write error: " << ec.message() << std::endl;
            ConnectionMgr::GetInstance()->RemoveConnection(_uuid);
        }
    });
}