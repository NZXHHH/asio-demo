#include"LogicSystem.h"

//void LogicSystem::PostMsgToQue(shared_ptr<CSession> _session, short msg_id, string data)
//{
//	std::unique_lock<std::mutex> loc(_mutex);
//	_que.push(make_shared<SendNode>(data, data.length(),msg_id));
//	if (_que.size()>1)
//	{
//		_consume.wait(loc);
//	}
//
//}

LogicSystem::LogicSystem():_b_stop(false)
{
	RegisterCallBack();
	_thread = std::thread (&LogicSystem::DealMsg, this);
}
LogicSystem::~LogicSystem()
{
	_b_stop = true;
	_consume.notify_one();
	_thread.join();
}
void LogicSystem::PostMsgToQue(shared_ptr<LogicNode> msg)
{
	std::unique_lock<std::mutex> unique_lk(_mutex);
	_que.push(msg);
	//将0置为1，用于通知信号
	if (_que.size() == 1) {
		unique_lk.unlock();
		_consume.notify_one();
	}
}

void LogicSystem::RegisterCallBack()
{
	//_handle_func_set.emplace(make_pair<SHOW, ShowCallBack>);
	_handle_func_set[SHOW] = bind(&LogicSystem::ShowCallBack, this, placeholders::_1, placeholders::_2, placeholders::_3);
}

void LogicSystem::ShowCallBack(shared_ptr<CSession> _session, short msg_id, string data)
{
	// 将接收到的消息原封不动地传回去
	//_session->send((char*)data.data(), static_cast<int>(data.length()), msg_id);
	Json::Reader reader;
	Json::Value root;
	reader.parse(data, root);
	std::cout << "recevie msg id  is " << root["id"].asInt() << " msg data is "
		<< root["data"].asString() << endl;
	root["data"] = "server has received msg, msg data is " + root["data"].asString();
	std::string return_str = root.toStyledString();
	_session->send((char*)return_str.data(), static_cast<int>(return_str.length()), root["id"].asInt());
}

void LogicSystem::DealMsg()
{
	
	while (1)
	{
		std::unique_lock<std::mutex> loc(_mutex);
		//使用if，因为唤醒之后要再次通过while再次判断，以防虚假唤醒
		while (_que.empty() && !_b_stop) {
			_consume.wait(loc);
		}
		if (_b_stop)
		{
			while (!_que.empty())
			{
				auto top_msg = _que.front();
				auto temp_handle = _handle_func_set.find(top_msg->_recvnode->_msg_id);
				if (temp_handle == _handle_func_set.end())
				{
					_que.pop();
					continue;
				}
				else
				{
					
					temp_handle->second(top_msg->_session, top_msg->_recvnode->_msg_id, top_msg->_recvnode->_data);
					_que.pop();
				}
			}
			break;
		}
		
		auto top_msg = _que.front();
		auto temp_handle = _handle_func_set.find(top_msg->_recvnode->_msg_id);
		if (temp_handle == _handle_func_set.end())
		{
			_que.pop();
			continue;
		}
		else
		{
			//CSession怎么获取
			temp_handle->second(top_msg->_session, top_msg->_recvnode->_msg_id, top_msg->_recvnode->_data);
			_que.pop();
		}
	}
}

LogicNode::LogicNode(shared_ptr<CSession> _session, shared_ptr<RecvNode> _recvnode) :_session(_session), _recvnode(_recvnode)
{
}