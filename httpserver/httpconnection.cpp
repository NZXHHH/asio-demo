#include "httpconnection.h"
namespace my_program_state
{
	std::size_t request_count()
	{
		static std::size_t count = 0;
		return ++count;
	}

	std::time_t now()
	{
		return std::time(0);
	}
}
http_connection::http_connection(tcp::socket socket)
    : _socket(std::move(socket))
{
}
void http_connection::check_deadline()
{
    deadline_.async_wait([self = shared_from_this()](boost::system::error_code ec)
    {
        if (!ec)
        {
            self->_socket.close();
        }
    });
}
void http_connection::start()
{
    boost::beast::http::async_read(_socket, _buffer, _request,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                self->check_deadline();
                self->handle_request();
            }
        });
}

void http_connection::handle_request()
{
    _response.version(_request.version());
    _response.keep_alive(_request.keep_alive());
    switch (_request.method())
    {
        case http::verb::get:
            _response.result(http::status::ok);
            _response.set(http::field::server, "Boost.Beast");
            create_response();
            break;
        case http::verb::post:
            _response.result(http::status::ok);
            _response.set(http::field::server, "Boost.Beast");
            create_post_response();
            break;
        default:
            _response.result(http::status::bad_request);
            _response.set(http::field::server, "text/plain");
            _response.body() = "Invalid request-method '" + std::string(_request.method_string()) + "'";
            break;
    }
    write_response();
}

void http_connection::create_response()
{
    if (_request.target() == "/count")
		{
			_response.set(http::field::content_type, "text/html");
			_response.body() =
				"<html>\n"
				"<head><title>Request count</title></head>\n"
				"<body>\n"
				"<h1>Request count</h1>\n"
				"<p>There have been "
				+ std::to_string(my_program_state::request_count())
				+ " requests so far.</p>\n"
				"</body>\n"
				"</html>\n";
		}
		else if (_request.target() == "/time")
		{
			_response.set(http::field::content_type, "text/html");
			_response.body() =
				"<html>\n"
				"<head><title>Current time</title></head>\n"
				"<body>\n"
				"<h1>Current time</h1>\n"
				"<p>The current time is "
				+ std::to_string(my_program_state::now())
				+ " seconds since the epoch.</p>\n"
				"</body>\n"
				"</html>\n";
		}
		else
		{
			_response.result(http::status::not_found);
			_response.set(http::field::content_type, "text/plain");
			_response.body() = "File not found\r\n";
		}
}

void http_connection::create_post_response()
{
    if(_request.target()!="/email")
    {
        _response.result(http::status::not_found);
        _response.set(http::field::content_type, "text/plain");
        _response.body() = "File not found\r\n";
        return;
    }
    auto& body_str = _request.body(); 
    std::cout<<"body_str:"<<body_str<<std::endl;
    Json::Reader reader;
    Json::Value root;
    Json::Value src_root;
    bool parsingSuccessful = reader.parse(body_str, src_root);
    if (!parsingSuccessful)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = "Failed to parse JSON data!";
        _response.result(http::status::bad_request);
        _response.set(http::field::content_type, "application/json");
        _response.body() = root.toStyledString();
        return;
    }
    auto email = src_root["email"];
    std::cout<<"email:"<<email<<std::endl;
    root["error"]=0;
    root["email"]=src_root["email"];
    _response.set(http::field::content_type, "application/json");
    _response.body() = root.toStyledString();

}

void http_connection::write_response()
{
    _response.content_length(_response.body().size());
    boost::beast::http::async_write(_socket, _response,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                self->_socket.shutdown(tcp::socket::shutdown_send, ec);
                self->deadline_.cancel();
            }
        });
}