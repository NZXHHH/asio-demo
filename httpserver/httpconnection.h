#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
    http_connection(tcp::socket socket);
    void start();
    void handle_request();
    void create_response();
    void create_post_response();
    void write_response();
    void check_deadline();
private:
    tcp::socket _socket;
    http::request<http::string_body> _request;
    http::response<http::string_body> _response;
    beast::flat_buffer _buffer;
    net::steady_timer deadline_{
		_socket.get_executor(), std::chrono::seconds(60) };
};  