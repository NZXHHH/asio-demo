
#include"CServer.h"
#include"Session.h"
#include"MsgNode.h"
void CServer::start_accept()
{
	shared_ptr<CSession> session = make_shared<CSession>(AsioIOThreadPool::GetInstance()->get_io_service(), this);
	acceptor.async_accept(session->Socket(), bind(&CServer::handle_accept, this, session, placeholders::_1));
}

void CServer::handle_accept(shared_ptr<CSession> session, boost::system::error_code error)
{
	if (!error)
	{
		session->Start();
		_sessions.insert(make_pair(session->GetUuid(), session));

	}
	start_accept();

}
void CServer::ClearSession(std::string uuid) {
	_sessions.erase(uuid);
}