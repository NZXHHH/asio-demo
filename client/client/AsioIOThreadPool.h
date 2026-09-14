#pragma once
#include <boost/asio.hpp>
#include <vector>
#include"Singleton.h"
using namespace std;
using IOService = boost::asio::io_context;
using Work=boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
using WorkPtr=std::unique_ptr<Work>;
class AsioIOThreadPool : public Singleton<AsioIOThreadPool>
{
    friend class Singleton<AsioIOThreadPool>;
public:
    AsioIOThreadPool(size_t pool_size=std::thread::hardware_concurrency());
    ~AsioIOThreadPool();
    boost::asio::io_context& get_io_service();
    void Stop();
private:
    boost::asio::io_context _io_services;
    WorkPtr _works;
    std::vector<std::thread> _threads;
};