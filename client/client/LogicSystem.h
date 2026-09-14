#pragma once
#include"MsgNode.h"
#include"Session.h"
#include"Singleton.h"
#include <json/json.h>
#include <json/value.h>
#define SHOW 1001
class LogicNode
{
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};

typedef function<void(shared_ptr<CSession>, short msg_id, string data)> FuncCallBack;
class LogicSystem :public Singleton<LogicSystem>
{
	friend class LogicSystemTest;
public:
	friend class Singleton<LogicSystem>;
	
	~LogicSystem();
	//void PostMsgToQue(shared_ptr<CSession>, short msg_id, string data);
	void PostMsgToQue(shared_ptr < LogicNode> msg);
	
	void RegisterCallBack();
	void ShowCallBack(shared_ptr<CSession>, short msg_id, string data);
	void DealMsg();
private:
	LogicSystem();
	std::mutex _mutex;
	std::condition_variable _consume;
	map<short, FuncCallBack> _handle_func_set;
	std::thread _thread;
	bool _b_stop;

	//std::queue<shared_ptr<SendNode>> _que;
	std::queue<shared_ptr<LogicNode>> _que;
};