#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(net::io_context &ioc, unsigned short port)
: _ioc(ioc)
, _acceptor(ioc, net::ip::tcp::endpoint(net::ip::tcp::v4(), port))
{
    StartAccept();
}

void WebSocketServer::StartAccept()
{
    std::shared_ptr<Connection> soc=make_shared<Connection>(_ioc);
    _acceptor.async_accept(soc->GetSocket(), [this, soc](const boost::system::error_code &ec)
    {
        if (!ec)
        {
            soc->Start();
            ConnectionMgr::GetInstance()->AddConnection(soc);
        }
        StartAccept();
    });
}
int main(int argc, char* argv[])
{
	try
	{
		unsigned short port = static_cast<unsigned short>(8080);
		net::io_context ioc;
		//auto endpoint = boost::asio::ip::tcp::endpoint{ net::ip::make_address("0.0.0.0"), port };
		WebSocketServer server(ioc, port);
		std::cout << "Server listening on http://localhost:" << port << std::endl;
		ioc.run();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
