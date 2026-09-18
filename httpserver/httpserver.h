#pragma once
#include <boost/asio.hpp>
class http_connection;
class http_server
{
public:
    http_server(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint);
    void do_accept();
private:
    boost::asio::io_context& _ioc;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
};