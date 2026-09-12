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
#include"MsgNode.h"
#include"const.h"
class CServer;
class CSession:public enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& ios, CServer* server);
	void Start();
	void handle_write(boost::system::error_code, std::size_t,shared_ptr<CSession>);
	void handle_read(boost::system::error_code, std::size_t, shared_ptr<CSession>);
	void HandleReadMsg(boost::system::error_code, std::size_t, shared_ptr<CSession>);
	tcp::socket& Socket() { return _soc; };
	void send(char* buf, int length, int msg_id);
	void Clear();
	std::string& GetUuid();
private:
	tcp::socket _soc;
	char _data[MAX_LENGTH];
	std::mutex _send_lock;
	std::queue<shared_ptr<MsgNode>> que;
	CServer* _server;
	std::string _uuid;
	shared_ptr<MsgNode>_head_node;
	shared_ptr<RecvNode>_recv_msg_node;
};