#pragma once
#include <boost/asio.hpp>
#include <vector>
#include"Singleton.h"
using namespace std;
using IOService = boost::asio::io_context;
using Work=boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
using WorkPtr=std::unique_ptr<Work>;
class AsioIOServicePool : public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;
public:
    AsioIOServicePool(size_t pool_size=std::thread::hardware_concurrency());
    ~AsioIOServicePool();
    boost::asio::io_context& get_io_service();
    void Stop();
private:
    std::vector<IOService> _io_services;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
};