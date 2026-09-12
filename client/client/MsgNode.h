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
class MsgNode
{
public:
	MsgNode(const char* buf, int total_len) :total_len(total_len), cur_len(0)
	{
		_data = new char[total_len + 1];
		
		_data[total_len] = '\0';
	}
	MsgNode(int total_len) :total_len(total_len), cur_len(0)
	{
		_data = new char[total_len+1];
		_data[total_len] = '\0';
	}
	char* _data;
	int total_len;
	int cur_len;
};

class RecvNode:public MsgNode{
public:
	RecvNode(short max_len, short msg_id) :MsgNode(max_len), _msg_id(msg_id){}
	short _msg_id;
};
class SendNode :public MsgNode {
public:
	SendNode(char*buf,short max_len, short msg_id) :MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
	{
		short msgid_net = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
		short len_net = boost::asio::detail::socket_ops::host_to_network_short(max_len);
		memcpy(_data, &msgid_net, HEAD_ID_LEN);
		memcpy(_data + HEAD_ID_LEN, &len_net, HEAD_DATA_LEN);
		memcpy(_data + HEAD_TOTAL_LEN, buf, max_len);
	}
	short _msg_id;
};