#include "httpconnection.h"
#include "httpserver.h"

http_server::http_server(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint)
    : _ioc(ioc), _acceptor(ioc, endpoint), _socket(ioc)
{
    do_accept();
}

void http_server::do_accept()
{
    _acceptor.async_accept(_socket, [this](boost::system::error_code ec)
    {
        if (!ec)
        {
            std::make_shared<http_connection>(std::move(_socket))->start();
            _socket = tcp::socket(_ioc);
            do_accept();
        }
    });
}

int main(int argc, char* argv[])
{
	try
	{
		unsigned short port = static_cast<unsigned short>(8080);
		net::io_context ioc;
		auto endpoint = tcp::endpoint{ net::ip::make_address("0.0.0.0"), port };
		http_server server(ioc, endpoint);
		std::cout << "Server listening on http://localhost:" << port << std::endl;
		ioc.run();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}