#pragma once
#include "boost/asio.hpp"
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace std;
using  boost::asio::ip::tcp;
#include <queue>
#include<map>
#include"const.h"
#include"AsioIOServicePool.h"
#include"Session.h"
class CServer
{
public:
	CServer(boost::asio::io_context& ioc, short port) :ioc(ioc), acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
		start_accept();
	};
	void start_accept();
	void handle_accept(shared_ptr<CSession>session, boost::system::error_code error);
	void ClearSession(std::string uuid);
private:
	boost::asio::io_context& ioc;
	tcp::acceptor acceptor;
	std::map<std::string, shared_ptr<CSession>> _sessions;
};