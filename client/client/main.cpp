#include <iostream>
#include <boost/asio.hpp>
#include "Session.h"
#include "CServer.h"
#include "MsgNode.h"
//using namespace std;
int main()
{
	try {
		cout<<"IOthreadPool"<<endl;
		auto pool = AsioIOThreadPool::GetInstance();
		boost::asio::io_context  io_context;
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&io_context,pool](auto, auto) {
			io_context.stop();
			pool->Stop();
			});
		CServer s(io_context, 10086);
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << endl;
	}

}