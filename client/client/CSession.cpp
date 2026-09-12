#include "CServer.h"
#include "LogicSystem.h"
CSession::CSession(boost::asio::io_context& ios, CServer* server) :_soc(ios), _server(server) {
	boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
	_uuid = boost::uuids::to_string(a_uuid);
	_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
};
void CSession::Start()
{
	
	boost::asio::async_read(_soc,boost::asio::buffer(_head_node->_data, HEAD_TOTAL_LEN), bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, shared_from_this()));
	
}

void CSession::handle_write(boost::system::error_code error, std::size_t, shared_ptr<CSession> session)
{
	if (!error)
	{
		std::lock_guard<std::mutex> lock(_send_lock);
		que.pop();
		if (!que.empty())
		{
			auto top_msg = que.front();
			boost::asio::async_write(_soc,boost::asio::buffer(top_msg->_data, top_msg->total_len),
				bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, session
				));
		}
	}
	else
	{
		_server->ClearSession(_uuid);
	}
	
}

void CSession::handle_read(boost::system::error_code error, std::size_t tranfered, shared_ptr<CSession> session)
{
	if (!error)
	{
		short msg_id;
		short body_len;
		memcpy(&msg_id, _head_node->_data, HEAD_ID_LEN);
		memcpy(&body_len, _head_node->_data+ HEAD_ID_LEN, HEAD_DATA_LEN);
		msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
		body_len = boost::asio::detail::socket_ops::network_to_host_short(body_len);
		if (body_len > MAX_LENGTH || body_len <= 0)
		{
			_server->ClearSession(session->GetUuid());
			return;
		}
		_recv_msg_node = make_shared<RecvNode>(body_len, msg_id);
		boost::asio::async_read(_soc, boost::asio::buffer(_recv_msg_node->_data, body_len), bind(&CSession::HandleReadMsg, this, placeholders::_1, placeholders::_2, shared_from_this()));
	
	}
	else
	{
		_server->ClearSession(session->GetUuid());
	}
}

void CSession::HandleReadMsg(boost::system::error_code error, std::size_t tranfered, shared_ptr<CSession> session)
{
	if (!error)
	{
		LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
		boost::asio::async_read(_soc, boost::asio::buffer(_head_node->_data, HEAD_TOTAL_LEN), bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, shared_from_this()));

	}
	else {
		_server->ClearSession(_uuid);
	}
}

void CSession::send(char* buf,int length,int msg_id)
{
	std::lock_guard<std::mutex> lock(_send_lock);
	que.push(make_shared<SendNode>(buf, length,msg_id));
	if (que.size() == 1)
	{
		auto first_msg = que.front();
		boost::asio::async_write(_soc,boost::asio::buffer(first_msg->_data, first_msg->total_len),
			bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, shared_from_this()
			));
	}

}

void CSession::Clear()
{
}

std::string& CSession::GetUuid()
{
	return _uuid;
}